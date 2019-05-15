#pragma once

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <type_traits>
#include <unordered_set>

#include "convert.h"
#include "options.h"
#include "parser.h"
#include "std_optional.h"

namespace argparse {

class Parser {
  public:
    struct Callbacks {
        std::function<void()> exit;
        std::function<void()> help;
        std::function<void(const std::string &)> missing;
        std::function<void(const std::string &, const std::vector<std::string> &)> invalid;
    };

    explicit Parser(std::string name = "", std::string help = "")
        : name_(std::move(name)), help_(std::move(help)) {
        // Add a help option by default
        add<bool>({
            .name = "help",
            .letter = 'h',
            .help = "Show help message",
            .required = false,
            .default_value = false,
        });

        set_default_callbacks();
    }

    template <typename T>
    ConstPlaceHolder<std::vector<T>> add_multivalent(Option::Config<T> config) {
        static_assert(detail::acceptable<T>(), "Must be a valid type");

        // Check and update name
        validate<T>(config);

        return options_.add_multivalent<T>(std::move(config));
    }

    template <typename T>
    ConstPlaceHolder<T> add(Option::Config<T> config) {
        static_assert(detail::acceptable<T>(), "Must be a valid type");

        // Check and update name
        validate<T>(config);

        return options_.add<T>(std::move(config));
    }

    /// Add overload to specify individual arguments
    /// Non const inputs are moved
    template <typename T>
    ConstPlaceHolder<T> add(std::string name, // NOLINT(performance-unnecessary-value-param)
                            std::string help, // NOLINT(performance-unnecessary-value-param)
                            const char letter = Option::kUnusedChar,
                            const bool required = false,
                            pstd::optional<T> default_value = T{},
                            std::unordered_set<T> allowed_values = {}) {
        static_assert(detail::acceptable<T>(), "Must be a valid type");

        Option::Config<T> config{
            .name = std::move(name),
            .help = std::move(help),
            .letter = letter,
            .required = required,
            .default_value = std::move(default_value),
            .allowed_values = std::move(allowed_values)
        };

        // Check and update name
        validate<T>(config);

        return options_.add<T>(std::move(config));
    }

    template <typename T>
    ConstPlaceHolder<T> add_leading_positional(Option::Config<T> config) {
        static_assert(detail::acceptable<T>(), "Must be a valid type");

        // Check and update name
        validate<T>(config);

        // Save position
        positionals_.push_back(config.name);

        // All leading positional arguments are required, otherwise the positions invalidates others
        config.required = true;
        config.positional = true;
        auto placeholder = options_.add<T>(std::move(config));

        return placeholder;
    }

    /// For each valid callback, sets it
    /// Ignores null callback objects so existing callback will not be overwritten
    void set_callbacks(Callbacks &&cbs) {
        auto move_if_exists = [](auto &src, auto &dest) {
            if (src) {
                dest = std::move(src);
            }
        };

        move_if_exists(cbs.exit, cbs_.exit);
        move_if_exists(cbs.help, cbs_.help);
        move_if_exists(cbs.missing, cbs_.missing);
        move_if_exists(cbs.invalid, cbs_.invalid);
    }

    void help() const {
        // Separator
        std::cout << '\n';

        // Usage
        std::stringstream ss;
        ss << "Usage: ";
        ss << name_ << " ";
        for (const auto &name : positionals_) {
            ss << "[" << name << "]" << " ";
        }
        ss << options_.usage_string();
        std::cout << ss.str() << '\n';

        // Help / description
        std::cout << "Description: " << help_ << '\n';

        // Separator
        std::cout << '\n';

        // Options table
        std::cout << "Options:\n" << options_.display_string() << std::endl;

        cbs_.help();
    }

    /// Parse arguments
    /// \return Remaining, non-parsed arguments
    const std::vector<std::string> &parse(const int argc, const char **argv) {
        assert(argv);
        return parse(argc, argv, true);
    }

    std::unordered_map<std::string, Parser> &add_subparser(std::string &&group,
                                                           std::unordered_set<std::string> &&allowed_values) {
        if (subparser_.has_value()) {
            log_error("Can only register one subparser per parser");
            cbs_.exit();
            return subparser_.value();
        }

        subparser_group_ = std::move(group);
        subparser_.emplace();

        for (auto &&av : allowed_values) {
            subparser_->insert({av, Parser(av)});
        }

        return subparser_.value();
    }

    const std::string &subparser() const {
        return selected_subparser_;
    }

  private:
    const std::string name_ = "<PROGRAM_NAME>";
    const std::string help_ = "<HELP_MESSAGE>";

    Callbacks cbs_;
    Options options_;
    std::vector<std::string> remaining_args_;
    detail::Parser parser_;
    std::unordered_set<std::string> existing_args_;

    /// Positional arguments that go before other arguments
    std::vector<std::string> positionals_;

    /// Map of subparser name to subparser
    pstd::optional<std::unordered_map<std::string, Parser>> subparser_;

    /// Name of the subparser group
    pstd::optional<std::string> subparser_group_;

    /// Chosen subparser from the subparser group
    std::string selected_subparser_;

    template <typename T>
    void validate(Option::Config<T> &config) {
        const bool not_using_char = (config.letter == Option::kUnusedChar);
        const bool invalid_name = (config.name.length() == 1);
        const bool no_name = config.name.empty();

        // If not using char, the name must be valid
        if (not_using_char) {
            if (invalid_name || no_name) {
                throw InvalidConfig{};
            }
        } else {
            // If using char, the name must be valid or empty
            if (invalid_name) {
                throw InvalidConfig{};
            }

            // If the name is empty, then the name will become the letter
            if (no_name) {
                config.name += config.letter;
            }
        }

    }

    /// Parse arguments
    /// \param pop_first To remove the first argument or not
    /// \return          Remaining, non-parsed arguments
    const std::vector<std::string> &parse(const int argc, const char **argv, const bool pop_first) {
        // If pop first, decrement size, increment pointer
        const int new_argc = (pop_first) ? (argc - 1) : (argc);
        const char **new_argv = (pop_first) ? (argv + 1) : (argv);

        // If subparsers exists
        if (subparser_.has_value()) {
            // Check for subparser option, should have at least one argument
            if (new_argc <= 0) {
                cbs_.missing(subparser_group_.value());
                return remaining_args_;
            }

            // Select subparser
            selected_subparser_ = new_argv[0];
            const auto iterator = subparser_->find(selected_subparser_);
            if (iterator == subparser_->end()) {
                cbs_.invalid(subparser_group_.value(), {selected_subparser_});
                help();
                cbs_.exit();
                return remaining_args_;
            }

            // Let the subparser do the remainder of the parsing
            auto &parser = (*subparser_)[selected_subparser_];
            return parser.parse(new_argc, new_argv, true);
        }

        cross_check(parser_.parse(new_argc, new_argv));

        // Print help if requested
        const bool print_help = existing_args_.find("h")    != existing_args_.end() ||
                                existing_args_.find("help") != existing_args_.end();
        if (print_help) {
            help();
            cbs_.exit();
        }

        // Check against required arguments
        if (!check_requirements()) {
            help();
            cbs_.exit();
        }

        return remaining_args_;
    }

    void cross_check(Args &&args) {
        bool any_invalid = false;

        // Check positional arguments
        const auto &positional_args = parser_.positional_args();
        for (std::size_t position = 0; position < positionals_.size(); position++) {
            const auto &name = positionals_[position];

            // Check if any value exists over there
            if (position < positional_args.size()) {
                // Option should exist, the name is the same name that was used to add the option
                auto option = options_.get(name);
                assert(option);

                // Mark as existing
                existing_args_.insert(name);

                // Set the value
                const auto &value = positional_args[position];
                if (!option->set(value)) {
                    cbs_.invalid(name, {value});
                    any_invalid = true;
                }
            } else {
                log_error("Expected positional argument [", name, "] at", position, "position");
                help();
                cbs_.exit();
            }
        }

        // Check non positional arguments
        for (auto &pair : args) {
            const auto &name = pair.first;
            auto &values = pair.second;

            if (name == detail::Parser::kRemainingArgsKey) {
                remaining_args_ = std::move(values);
                continue;
            }

            // Did not find option
            auto option = options_.get(name);
            if (!option) {
                continue;
            }

            // Don't check for positional arguments here
            if (option->positional()) {
                continue;
            }

            // Boolean parameter just checks if the flag exists or not
            if (option->type() == Variant::Type::kBool) {
                if (!option->set("true")) {
                    cbs_.invalid(name, {"true"});
                    any_invalid = true;
                }
                existing_args_.insert(option->name());
                continue;
            }

            // No values for the option
            if (values.empty()) {
                cbs_.missing(name);
                any_invalid = true;
                continue;
            }

            // This option has a value, add it to set of args
            existing_args_.insert(option->name());

            // Multivalent, set all values
            if (option->multivalent()) {
                if (!option->set(values)) {
                    cbs_.invalid(name, values);
                    any_invalid = true;
                }
                continue;
            }

            // Not multivalent, should not have more than 1 value
            if (values.size() > 1) {
                cbs_.invalid(name, values);
                any_invalid = true;
            } else {
                // Not multivalent, only one value
                if (!option->set(values[0])) {
                    cbs_.invalid(name, {values[0]});
                    any_invalid = true;
                }
            }
        }

        if (any_invalid) {
            help();
            cbs_.exit();
        }
    }

    bool check_requirements() const {
        const auto &missings = options_.check_requirements(existing_args_);
        for (const auto &pair : missings) {
            cbs_.missing(pair.first);
        }

        return missings.empty();
    }

    /// Set the default callbacks into [cbs_]
    void set_default_callbacks() {
        cbs_.exit = [] { throw std::runtime_error("Parsing failed"); };
        cbs_.help = [] { };
        cbs_.missing = [this](const auto &s) {
            log_error("Missing required argument : ", s);
        };
        cbs_.invalid = [this](const auto &name, const auto &values) {
            std::string values_combined = "{";
            for (const auto &value : values) {
                values_combined += " " + value;
            }
            values_combined += " }";
            const char *v = values.empty() ? "???" : values_combined.c_str();
            log_error("Argument invalid : [ --", name, "=", v, "]");
        };
    }

    template <typename ... Args>
    void log_error(Args && ... args) const {
        static constexpr char kRedColorCode[] = "\033[0;31m";
        static constexpr char kRevertColorCode[] = "\033[0m";
        static constexpr char kPrefix[] = "[Parsing Error] ";
        std::stringstream ss;
        ss << kRedColorCode << kPrefix;
        log_error(ss, std::forward<Args>(args)...);
        std::cout << ss.str() << kRevertColorCode;
    }

    template <typename Arg, typename ... Args>
    void log_error(std::stringstream &ss, Arg && first, Args && ... args) const {
        ss << first << " ";
        log_error(ss, std::forward<Args>(args)...);
    }

    void log_error(std::stringstream &ss) const {
        ss << "\n";
    }
};

} // namespace argparse
