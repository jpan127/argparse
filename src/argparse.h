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
        return options_.add_multivalent<T>(std::move(config));
    }

    template <typename T>
    ConstPlaceHolder<T> add(Option::Config<T> config) {
        static_assert(detail::acceptable<T>(), "Must be a valid type");
        return options_.add<T>(std::move(config));
    }

    /// Add overload to specify individual arguments
    /// Non const inputs are moved
    template <typename T>
    ConstPlaceHolder<T> add(std::string name, // NOLINT(performance-unnecessary-value-param)
                            std::string help, // NOLINT(performance-unnecessary-value-param)
                            const char letter = Option::Config<T>::kUnusedChar,
                            const bool required = false,
                            pstd::optional<T> default_value = T{},
                            std::unordered_set<T> allowed_values = {}) {
        static_assert(detail::acceptable<T>(), "Must be a valid type");
        return options_.add<T>({
            .name = std::move(name),
            .help = std::move(help),
            .letter = letter,
            .required = required,
            .default_value = std::move(default_value),
            .allowed_values = std::move(allowed_values)
        });
    }

    template <typename T, int32_t Position>
    ConstPlaceHolder<T> add_leading_positional(Option::Config<T> config) {
        static_assert(Position > 0, "Position is 1-indexed");

        // Shift position to the left because the path argument will be stripped
        constexpr int32_t kPositionMinusOne = Position - 1;
        if (position_map_.find(kPositionMinusOne) != position_map_.end()) {
            std::cout << "Position " << Position
                      << " has already been taken, can not have multiple options in the same position\n";
            return nullptr;
        }

        // Save position
        position_map_[kPositionMinusOne] = config.name;

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
        std::cout
            << "Usage       : " << name_ << " [Options(s)]...\n"
            << "Description : " << help_ << '\n'
            << "Options     : " << std::endl;
        options_.display();

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
            std::cout << "Can only register one subparser per parser" << std::endl;
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
    const std::string name_ = "No Name";
    const std::string help_ = "No Help";

    Callbacks cbs_;
    Options options_;
    std::vector<std::string> remaining_args_;
    detail::Parser parser_;
    std::unordered_set<std::string> existing_args_;

    std::unordered_map<int32_t, std::string> position_map_;

    /// Map of subparser name to subparser
    pstd::optional<std::unordered_map<std::string, Parser>> subparser_;

    /// Name of the subparser group
    pstd::optional<std::string> subparser_group_;

    /// Chosen subparser from the subparser group
    std::string selected_subparser_;

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

    void cross_check(Args && args) {
        auto check = [this](auto &map) {
            // Converts char or std::string into std::string
            auto make_name_string = [](const auto &chars) {
                std::string s;
                s += chars;
                return s;
            };

            for (auto &pair : map) {
                const auto &name = pair.first;
                auto &values = pair.second;
                const auto name_string = make_name_string(name);

                if (name_string == detail::Parser::kRemainingArgsKey) {
                    remaining_args_ = std::move(values);
                    continue;
                }

                // Did not find option
                auto option = options_.get(name);
                if (!option) {
                    cbs_.invalid(name_string, values);
                    continue;
                }

                // Don't check for positional arguments here
                if (option->positional()) {
                    continue;
                }

                // Boolean parameter just checks if the flag exists or not
                if (option->type() == Variant::Type::kBool) {
                    if (!option->set("true")) {
                        cbs_.invalid(name_string, {"true"});
                    }
                    existing_args_.insert(option->name());
                    continue;
                }

                // No values for the option
                if (values.empty()) {
                    cbs_.missing(name_string);
                    continue;
                }

                // This option has a value, add it to set of args
                existing_args_.insert(option->name());

                // Multivalent, set all values
                if (option->multivalent()) {
                    if (!option->set(values)) {
                        cbs_.invalid(name_string, values);
                    }
                    continue;
                }

                // Not multivalent, should not have more than 1 value
                if (values.size() > 1) {
                    cbs_.invalid(name_string, values);
                } else {
                    // Not multivalent, only one value
                    if (!option->set(values[0])) {
                        cbs_.invalid(name_string, {values[0]});
                    }
                }
            }
        };

        const auto &positional_args = parser_.positional_args();
        for (const auto &pair : position_map_) {
            const auto &position = pair.first;
            const auto &name = pair.second;
            // Positive position, leading option
            if (position >= 0) {
                // Check if any value exists over there
                if (position < positional_args.size()) {
                    // Option should exist, the name is the same name that was used to add the option
                    auto option = options_.get(name);
                    assert(option);

                    // Mark as existing
                    existing_args_.insert(name);

                    // Set the value
                    const auto &value = positional_args[position];
                    std::cout << "Setting " << name << " to " << value << std::endl;
                    if (!option->set(value)) {
                        cbs_.invalid(name, {value});
                    }
                } else {
                    std::cout
                        << "Expected positional argument [" << name << "] at "
                        << position << " position" << std::endl;
                    cbs_.exit();
                }
            } else {
                // Not supporting trailing positional args yet
            }
        }

        check(args.letter_map());
        check(args.string_map());
    }

    bool check_requirements() const {
        bool met = true;
        for (const auto &pair : options_) {
            const auto &name = pair.first;
            const auto option = pair.second;
            if (option->required() && existing_args_.find(name) == existing_args_.end()) {
                cbs_.missing(std::string(name));
                met = false;
            }
        }
        return met;
    }

    /// Set the default callbacks into [cbs_]
    void set_default_callbacks() {
        cbs_.exit = [] { throw std::runtime_error("Parsing failed"); };
        cbs_.help = [] { };
        cbs_.missing = [](const auto &s) {
            std::cout << "Missing required argument : " << s << std::endl;
        };
        cbs_.invalid = [](const auto &name, const auto &values) {
            std::string values_combined = "{";
            for (const auto &value : values) {
                values_combined += " " + value;
            }
            values_combined += " }";
            const char *v = values.empty() ? "???" : values_combined.c_str();
            printf("Argument invalid : [ --%s = %s ]\n", name.c_str(), v);
        };
    }
};

} // namespace argparse
