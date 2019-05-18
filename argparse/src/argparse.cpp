#include "argparse.h"

#include "convert.h"
#include "exceptions.h"
#include "options.h"
#include "parser.h"
#include "variant.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <sstream>

namespace argparse {

namespace {

void log_error(std::stringstream &ss) {
    ss << "\n";
}

template <typename Arg, typename ... Args>
void log_error(std::stringstream &ss, Arg && first, Args && ... args) {
    ss << first << " ";
    log_error(ss, std::forward<Args>(args)...);
}

template <typename ... Args>
void log_error(Args && ... args) {
    static constexpr char kRedColorCode[] = "\033[0;31m";
    static constexpr char kRevertColorCode[] = "\033[0m";
    static constexpr char kPrefix[] = "[Parsing Error] ";
    std::stringstream ss;
    ss << kRedColorCode << kPrefix;
    log_error(ss, std::forward<Args>(args)...);
    std::cout << ss.str() << kRevertColorCode;
}

} // namespace

Parser::Parser(std::string name, std::string help)
    : name_(std::move(name)),
      help_(std::move(help)),
      options_(std::make_unique<Options>()),
      parser_(std::make_unique<detail::Parser>()) {
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

Parser::~Parser() = default;

template <typename T>
ConstPlaceHolder<std::vector<T>> Parser::add_multivalent(Config<T> config) {
    static_assert(supported<T>(), "Must be a valid type");

    // Check and update name
    validate<T>(config);

    return options_->add_multivalent<T>(std::move(config));
}

template <typename T>
ConstPlaceHolder<T> Parser::add(Config<T> config) {
    static_assert(supported<T>(), "Must be a valid type");

    // Check and update name
    validate<T>(config);

    return options_->add<T>(std::move(config));
}

template <typename T>
ConstPlaceHolder<T> Parser::add(std::string name, // NOLINT(performance-unnecessary-value-param)
                                std::string help, // NOLINT(performance-unnecessary-value-param)
                                const char letter,
                                const bool required,
                                pstd::optional<T> default_value,
                                std::unordered_set<T> allowed_values) {
    static_assert(supported<T>(), "Must be a valid type");

    Config<T> config{
        .name = std::move(name),
        .help = std::move(help),
        .letter = letter,
        .required = required,
        .default_value = std::move(default_value),
        .allowed_values = std::move(allowed_values)
    };

    // Check and update name
    validate<T>(config);

    return options_->add<T>(std::move(config));
}

template <typename T>
ConstPlaceHolder<T> Parser::add_leading_positional(Config<T> config) {
    static_assert(supported<T>(), "Must be a valid type");

    // Check and update name
    validate<T>(config);

    // Save position
    positionals_.push_back(config.name);

    // All leading positional arguments are required, otherwise the positions invalidates others
    config.required = true;
    config.positional = true;
    auto placeholder = options_->add<T>(std::move(config));

    return placeholder;
}

void Parser::set_callbacks(Callbacks &&cbs) {
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

void Parser::help() const {
    // Separator
    std::cout << '\n';

    // Usage
    std::stringstream ss;
    ss << "Usage: ";
    ss << name_ << " ";
    for (const auto &name : positionals_) {
        ss << "[" << name << "]" << " ";
    }
    ss << options_->usage_string();
    std::cout << ss.str() << '\n';

    // Help / description
    std::cout << "Description: " << help_ << '\n';

    // Separator
    std::cout << '\n';

    // Options table
    std::cout << "Options:\n" << options_->display_string() << std::endl;

    cbs_.help();
}

const std::vector<std::string> &Parser::parse(const int argc, const char **argv) {
    assert(argv);
    return parse(argc, argv, true);
}

std::unordered_map<std::string, Parser> &Parser::add_subparser(std::string &&group,
                                                       std::unordered_set<std::string> &&allowed_values) {
    if (subparser_.has_value()) {
        log_error("Can only register one subparser per parser");
        cbs_.exit();
        return subparser_.value();
    }

    subparser_group_ = std::move(group);
    subparser_.emplace();

    for (auto &&av : allowed_values) {
        subparser_.value().emplace(av, av);
    }

    return subparser_.value();
}

template <typename T>
void Parser::validate(Config<T> &config) {
    const bool not_using_char = (config.letter == kUnusedChar);
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

const std::vector<std::string> &Parser::parse(const int argc, const char **argv, const bool pop_first) {
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

    cross_check(parser_->parse(new_argc, new_argv));

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

void Parser::cross_check(Args &&args) {
    bool any_invalid = false;

    // Check positional arguments
    const auto &positional_args = parser_->positional_args();
    for (std::size_t position = 0; position < positionals_.size(); position++) {
        const auto &name = positionals_[position];

        // Check if any value exists over there
        if (position < positional_args.size()) {
            // Option should exist, the name is the same name that was used to add the option
            auto option = options_->get(name);
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
        auto option = options_->get(name);
        if (!option) {
            continue;
        }

        // Don't check for positional arguments here
        if (option->positional()) {
            continue;
        }

        // Boolean parameter just checks if the flag exists or not
        if (option->type() == Type::kBool) {
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

bool Parser::check_requirements() const {
    const auto &missings = options_->check_requirements(existing_args_);
    for (const auto &pair : missings) {
        cbs_.missing(pair.first);
    }

    return missings.empty();
}

/// Set the default callbacks into [cbs_]
void Parser::set_default_callbacks() {
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

/// @{ Explicit Instantiation
template ConstPlaceHolder<std::vector<std::string>> Parser::add_multivalent(Config<std::string> config);
template ConstPlaceHolder<std::vector<double>> Parser::add_multivalent(Config<double> config);
template ConstPlaceHolder<std::vector<float>> Parser::add_multivalent(Config<float> config);
template ConstPlaceHolder<std::vector<uint64_t>> Parser::add_multivalent(Config<uint64_t> config);
template ConstPlaceHolder<std::vector<int64_t>> Parser::add_multivalent(Config<int64_t> config);
template ConstPlaceHolder<std::vector<uint32_t>> Parser::add_multivalent(Config<uint32_t> config);
template ConstPlaceHolder<std::vector<int32_t>> Parser::add_multivalent(Config<int32_t> config);
template ConstPlaceHolder<std::vector<uint16_t>> Parser::add_multivalent(Config<uint16_t> config);
template ConstPlaceHolder<std::vector<int16_t>> Parser::add_multivalent(Config<int16_t> config);
template ConstPlaceHolder<std::vector<uint8_t>> Parser::add_multivalent(Config<uint8_t> config);
template ConstPlaceHolder<std::vector<int8_t>> Parser::add_multivalent(Config<int8_t> config);
template ConstPlaceHolder<std::vector<bool>> Parser::add_multivalent(Config<bool> config);
template ConstPlaceHolder<std::vector<char>> Parser::add_multivalent(Config<char> config);
template ConstPlaceHolder<std::string> Parser::add(Config<std::string>);
template ConstPlaceHolder<double> Parser::add(Config<double>);
template ConstPlaceHolder<float> Parser::add(Config<float>);
template ConstPlaceHolder<uint64_t> Parser::add(Config<uint64_t>);
template ConstPlaceHolder<int64_t> Parser::add(Config<int64_t>);
template ConstPlaceHolder<uint32_t> Parser::add(Config<uint32_t>);
template ConstPlaceHolder<int32_t> Parser::add(Config<int32_t>);
template ConstPlaceHolder<uint16_t> Parser::add(Config<uint16_t>);
template ConstPlaceHolder<int16_t> Parser::add(Config<int16_t>);
template ConstPlaceHolder<uint8_t> Parser::add(Config<uint8_t>);
template ConstPlaceHolder<int8_t> Parser::add(Config<int8_t>);
template ConstPlaceHolder<bool> Parser::add(Config<bool>);
template ConstPlaceHolder<char> Parser::add(Config<char>);
template ConstPlaceHolder<std::string> Parser::add(std::string, std::string, const char, const bool, pstd::optional<std::string>, std::unordered_set<std::string>);
template ConstPlaceHolder<double> Parser::add(std::string, std::string, const char, const bool, pstd::optional<double>, std::unordered_set<double>);
template ConstPlaceHolder<float> Parser::add(std::string, std::string, const char, const bool, pstd::optional<float>, std::unordered_set<float>);
template ConstPlaceHolder<uint64_t> Parser::add(std::string, std::string, const char, const bool, pstd::optional<uint64_t>, std::unordered_set<uint64_t>);
template ConstPlaceHolder<int64_t> Parser::add(std::string, std::string, const char, const bool, pstd::optional<int64_t>, std::unordered_set<int64_t>);
template ConstPlaceHolder<uint32_t> Parser::add(std::string, std::string, const char, const bool, pstd::optional<uint32_t>, std::unordered_set<uint32_t>);
template ConstPlaceHolder<int32_t> Parser::add(std::string, std::string, const char, const bool, pstd::optional<int32_t>, std::unordered_set<int32_t>);
template ConstPlaceHolder<uint16_t> Parser::add(std::string, std::string, const char, const bool, pstd::optional<uint16_t>, std::unordered_set<uint16_t>);
template ConstPlaceHolder<int16_t> Parser::add(std::string, std::string, const char, const bool, pstd::optional<int16_t>, std::unordered_set<int16_t>);
template ConstPlaceHolder<uint8_t> Parser::add(std::string, std::string, const char, const bool, pstd::optional<uint8_t>, std::unordered_set<uint8_t>);
template ConstPlaceHolder<int8_t> Parser::add(std::string, std::string, const char, const bool, pstd::optional<int8_t>, std::unordered_set<int8_t>);
template ConstPlaceHolder<bool> Parser::add(std::string, std::string, const char, const bool, pstd::optional<bool>, std::unordered_set<bool>);
template ConstPlaceHolder<char> Parser::add(std::string, std::string, const char, const bool, pstd::optional<char>, std::unordered_set<char>);
template ConstPlaceHolder<std::string> Parser::add_leading_positional(Config<std::string>);
template ConstPlaceHolder<double> Parser::add_leading_positional(Config<double>);
template ConstPlaceHolder<float> Parser::add_leading_positional(Config<float>);
template ConstPlaceHolder<uint64_t> Parser::add_leading_positional(Config<uint64_t>);
template ConstPlaceHolder<int64_t> Parser::add_leading_positional(Config<int64_t>);
template ConstPlaceHolder<uint32_t> Parser::add_leading_positional(Config<uint32_t>);
template ConstPlaceHolder<int32_t> Parser::add_leading_positional(Config<int32_t>);
template ConstPlaceHolder<uint16_t> Parser::add_leading_positional(Config<uint16_t>);
template ConstPlaceHolder<int16_t> Parser::add_leading_positional(Config<int16_t>);
template ConstPlaceHolder<uint8_t> Parser::add_leading_positional(Config<uint8_t>);
template ConstPlaceHolder<int8_t> Parser::add_leading_positional(Config<int8_t>);
template ConstPlaceHolder<bool> Parser::add_leading_positional(Config<bool>);
template ConstPlaceHolder<char> Parser::add_leading_positional(Config<char>);
/// @}

} // namespace argparse
