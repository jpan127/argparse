#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <type_traits>
#include <unordered_map>

#include "std_optional.h"
#include "convert.h"
#include "options.h"

namespace argparse {

class Args {
  public:
    void add(const std::string &name, const std::shared_ptr<VariantOption> option) {
        args_[name] = option;
    }

    template <typename T>
    pre_std::optional<T> get(const std::string &name) const {
        static_assert(detail::acceptable<T>(), "Must be a valid type");

        const auto iterator = args_.find(name);
        if (iterator != args_.end()) {
            return detail::convert<T>(iterator->second);
        }

        return {};
    }

    bool exists(const std::string &name) const {
        return args_.find(name) != args_.end();
    }

  private:
    std::unordered_map<std::string, std::shared_ptr<VariantOption>> args_;
};

class Parser {
  public:
    Parser() = default;
    Parser(const std::string &name, const std::string &help) : name_(name), help_(help){}

    template <typename T>
    void add(const Option &options, const T default_value = {}) {
        static_assert(detail::acceptable<T>(), "Must be a valid type");
        options_.add<T>(options, default_value);
    }

    void help() const {
        std::cout << "Usage       : " << name_ << " [Options(s)]...\n"
                  << "Description : " << help_ << '\n'
                  << "Options     : " << std::endl;
        options_.display();
    }

    const Args &parse(const int argc, const char **argv) {
        for (std::size_t ii = 1; ii < argc; ii++) {
            parse_arg(argv[ii]);
        }

        // Check against required arguments
        bool missing_any = false;
        for (const auto &pair : options_) {
            const auto &name = pair.first;
            if (!args_.exists(name)) {
                if (missing_callback_) {
                    missing_callback_(name);
                }
                missing_any = true;
            }
        }

        if (missing_any) {
            exit_callback_();
        }

        return args_;
    }

    void set_invalid_callback(std::function<void(const std::string &)> &&cb) {
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
    std::function<void()> exit_callback_ = [] { exit(-1); };
    std::function<void(const std::string &)> invalid_callback_;
    std::function<void(const std::string &)> missing_callback_;

    void parse_arg(std::string &&s) {
        // Look for the option
        if (parser_state_ == State::Option) {
            last_option_ = validate_option(std::forward<std::string>(s));
        } else {
            auto option = options_.get(last_option_);
            if (option) {
                std::cout << "Found : " << option->opt.name << " = " << s << std::endl;
                switch (option->variant) {
                case kString : option->value.emplace<std::string>(detail::convert_helper<std::string>(s)); break;
                case kUint64 : option->value.emplace<uint64_t>(detail::convert_helper<uint64_t>(s)); break;
                case kInt64  : option->value.emplace<int64_t>(detail::convert_helper<int64_t>(s)); break;
                case kDouble : option->value.emplace<double>(detail::convert_helper<double>(s)); break;
                case kFloat  : option->value.emplace<float>(detail::convert_helper<float>(s)); break;
                case kBool   : option->value.emplace<bool>(detail::convert_helper<bool>(s)); break;
                }
                args_.add(option->opt.name, option);
            } else {
                if (invalid_callback_) {
                    invalid_callback_(s);
                }
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
