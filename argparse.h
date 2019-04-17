#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <cassert>
#include <cstdlib>
#include <vector>

#include "convert.h"
#include "options.h"

namespace argparse {

class Args {
  public:
    template <typename T>
    T get(const std::string &name) const {
        for (auto &arg : args_) {
            if (arg->opt.name == name) {
                return mpark::get<T>(arg->value);
            }
        }
    }

    std::vector<std::shared_ptr<VariantOption>> args_;
};

class Parser {
  public:
    Parser() = default;
    Parser(const std::string &name, const std::string &help) : name_(name), help_(help){}

    template <typename T>
    void add(const Option &options, const T default_value = {}) {
        options_.add<T>(options, default_value);
    }

    void help() const {
        std::cout << "Usage       : " << name_ << " [Options(s)]...\n"
                  << "Description : " << help_ << '\n'
                  << "Options     : " << std::endl;
        options_.display();
    }

    const Args &
    parse(const int argc, const char **argv) {
        for (std::size_t ii = 1; ii < argc; ii++) {
            parse_arg(argv[ii]);
        }

        return args_;
    }

    /// @TODO : Allow the user to specify actions in erroneous situations
    void set_invalid_callback(const std::function<void(const std::string &)> &cb);
    void missing_callback(const std::function<void(const std::string &)> &cb);

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

    void parse_arg(std::string &&s) {
        // Look for the option
        if (parser_state_ == State::Option) {
            last_option_ = validate_option(std::forward<std::string>(s));
        } else {
            auto option = options_.get(last_option_);
            if (option) {
                std::cout << "Found : " << option->opt.name << " = " << s << std::endl;
                switch (option->variant) {
                case kString : option->value.emplace<std::string>(detail::convert<std::string>(s)); break;
                case kUint64 : option->value.emplace<uint64_t>(detail::convert<uint64_t>(s)); break;
                case kInt64  : option->value.emplace<int64_t>(detail::convert<int64_t>(s)); break;
                case kDouble : option->value.emplace<double>(detail::convert<double>(s)); break;
                case kFloat  : option->value.emplace<float>(detail::convert<float>(s)); break;
                case kBool   : option->value.emplace<bool>(detail::convert<bool>(s)); break;
                }
                args_.args_.push_back(option);
            } else {
                std::cout << "Invalid : " << s << std::endl;
            }
        }

        // Switch states
        parser_state_ = (parser_state_ == State::Option) ? (State::Value) : (State::Option);
    }

    std::string validate_option(std::string &&s) {
        auto terminate = [&s] {
            std::cout << s << "is not a valid option" << std::endl;
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

        return s;
    }
};

} // namespace argparse
