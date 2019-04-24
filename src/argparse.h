#pragma once

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <type_traits>

#include <stdio.h>

#include "args.h"
#include "convert.h"
#include "options.h"
#include "std_optional.h"

namespace argparse {

class Parser {
  public:
    Parser(const std::string &name = "", const std::string &help = "")
        : name_(name), help_(help) {
        // Add a help option by default
        add<bool>({
            .name = "help",
            .letter = 'h',
            .help = "Show help message",
            .required = false,
        }, false);
    }

    template <typename T>
    void add(const Option::Config &config) {
        static_assert(detail::acceptable<T>(), "Must be a valid type");
        options_.add<T>(config);
    }

    template <typename T>
    void add(const Option::Config &config, const T &default_value) {
        static_assert(detail::acceptable<T>(), "Must be a valid type");
        options_.add<T>(config, default_value);
    }

    void help() const {
        std::cout
            << "Usage       : " << name_ << " [Options(s)]...\n"
            << "Description : " << help_ << '\n'
            << "Options     : " << std::endl;
        options_.display();

        if (help_callback_) {
            help_callback_();
        }
    }

    const Args &parse(const int argc, const char **argv) {
        for (std::size_t ii = 1; ii < static_cast<std::size_t>(argc); ii++) {
            parse_arg(argv[ii]);
        }

        // Check against required arguments
        bool missing_any = false;
        for (const auto &pair : options_) {
            const auto &name = pair.first;
            const auto option = pair.second;
            if (!args_.exists(name) && option->required()) {
                if (missing_callback_) {
                    missing_callback_(name);
                }
                missing_any = true;
            }
        }

        if (missing_any) {
            help();
            exit_callback_();
        }

        const auto arg = args_.get<bool>("help");
        if (arg.has_value()) {
            help();
            exit_callback_();
        }

        return args_;
    }

    void set_invalid_callback(std::function<void(const std::string &, const std::string &)> &&cb) {
        assert(cb);
        invalid_callback_ = std::move(cb);
    }
    void set_missing_callback(std::function<void(const std::string &)> &&cb) {
        assert(cb);
        missing_callback_ = std::move(cb);
    }
    void set_exit_callback(std::function<void()> &&cb) {
        assert(cb);
        exit_callback_ = std::move(cb);
    }
    void set_help_callback(std::function<void()> &&cb) {
        assert(cb);
        help_callback_ = std::move(cb);
    }

  private:
    enum class State {
        Option,
        Value,
    };

    const std::string name_ = "No Name";
    const std::string help_ = "No Help";
    Options options_;
    State parser_state_ = State::Option;
    std::string last_option_;
    Args args_;
    std::function<void()> help_callback_;
    std::function<void()> exit_callback_ = [] { exit(-1); };
    std::function<void(const std::string &, const std::string &)> invalid_callback_ =
        [](const std::string &name, const std::string &value) {
            const char *v = value.empty() ? "???" : value.c_str();
            printf("Argument invalid : [--%s=%s]\n", name.c_str(), v);
        };
    std::function<void(const std::string &)> missing_callback_ = [](const std::string &s) {
        std::cout << "Missing required argument : " << s << std::endl;
    };

    void parse_arg(std::string &&s) {
        auto get_option = [this] {
            return (last_option_.length() == 1)    ?
                   (options_.get(last_option_[0])) : // If single char, use char overload
                   (options_.get(last_option_));     // Else use string overload
        };

        // Look for the option
        if (parser_state_ == State::Option) {
            last_option_ = validate_option(std::forward<std::string>(s));

            const auto option = get_option();
            // If the option is boolean, then dont parse next token as value, it is also an option
            if (option) {
                if (option->type() == kBool) {
                    option->set("true");
                    args_.add(option->name(), option);
                    return;
                }
            } else {
                if (invalid_callback_) {
                    invalid_callback_(last_option_, s);
                }
            }
        } else {
            auto option = get_option();
            if (option) {
                option->set(std::move(s));
                args_.add(option->name(), option);
            } else {
                if (invalid_callback_) {
                    invalid_callback_(last_option_, s);
                }
            }
        }

        // Switch states
        parser_state_ = (parser_state_ == State::Option) ? (State::Value) : (State::Option);
    }

    std::string validate_option(std::string &&s) {
        auto terminate = [this, &s] {
            std::cout << s << " is not a valid option" << std::endl;
            help();
            std::exit(-1);
        };

        if (s.length() < 2) {
            terminate();
        } else if (s[0] != '-') {
            terminate();
        }

        bool short_form = true;

        s.erase(0, 1);
        if (s[0] == '-') {
            s.erase(0, 1);
            short_form = false;
        }

        if (short_form && s.length() != 1) {
            terminate();
        }

        // Validate characters
        auto isalpha = [](const char c) {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
        };
        for (const auto c : s) {
            if (!isalpha(c)) {
                terminate();
            }
        }

        assert(s.length() >= 1);

        // Non-pessimizing move since [s] is an rvalue reference
        return std::move(s);
    }
};

} // namespace argparse
