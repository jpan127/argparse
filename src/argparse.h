#pragma once

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <type_traits>

#include <stdio.h>

#include "convert.h"
#include "options.h"
#include "parser.h"
#include "std_optional.h"

namespace argparse {

class Parser {
  public:
    struct Callbacks {
        std::function<void()> help;
        std::function<void()> exit = [] { ::exit(-1); };
        std::function<void(const std::string &, const std::vector<std::string> &)> invalid =
            [](const std::string &name, const std::vector<std::string> &values) {
                std::string values_combined = "{";
                for (const auto &value : values) {
                    values_combined += " " + value;
                }
                values_combined += " }";
                const char *v = values.empty() ? "???" : values_combined.c_str();
                printf("Argument invalid : [ --%s = %s ]\n", name.c_str(), v);
            };
        std::function<void(const std::string &)> missing =
            [](const std::string &s) {
                std::cout << "Missing required argument : " << s << std::endl;
            };
    };

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
    std::shared_ptr<const T> add(const Option::Config &config) {
        static_assert(detail::acceptable<T>(), "Must be a valid type");
        return options_.add<T>(config);
    }

    template <typename T>
    std::shared_ptr<const T> add(const Option::Config &config, const T &default_value) {
        static_assert(detail::acceptable<T>(), "Must be a valid type");
        return options_.add<T>(config, default_value);
    }

    void help() const {
        std::cout
            << "Usage       : " << name_ << " [Options(s)]...\n"
            << "Description : " << help_ << '\n'
            << "Options     : " << std::endl;
        options_.display();

        if (cbs_.help) {
            cbs_.help();
        }
    }

    const Args &parse(const int argc, const char **argv) {
        cross_check(parser_.parse(argc, argv));

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

    void set_invalid_callback(std::function<void(const std::string &, const std::vector<std::string> &)> &&cb) {
        assert(cb);
        cbs_.invalid = std::move(cb);
    }
    void set_missing_callback(std::function<void(const std::string &)> &&cb) {
        assert(cb);
        cbs_.missing = std::move(cb);
    }
    void set_exit_callback(std::function<void()> &&cb) {
        assert(cb);
        cbs_.exit = std::move(cb);
    }
    void set_help_callback(std::function<void()> &&cb) {
        assert(cb);
        cbs_.help = std::move(cb);
    }

  private:
    const std::string name_ = "No Name";
    const std::string help_ = "No Help";

    Callbacks cbs_;
    Options options_;
    Args remaining_args_;
    detail::Parser parser_;
    std::unordered_set<std::string> existing_args_;

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

                auto option = options_.get(name);
                if (option) {
                    if (option->type() == kBool) {
                        option->set("true");
                        existing_args_.insert(option->name());
                    } else {
                        if (values.empty()) {
                            if (cbs_.missing) {
                                cbs_.missing(name_string);
                            }
                        } else {
                            // Only supporting one value right now
                            option->set(values[0]);
                            existing_args_.insert(option->name());
                        }
                    }
                } else {
                    if (cbs_.invalid) {
                        cbs_.invalid(name_string, values);
                    }

                    remaining_args_.create(name, std::move(values));
                }
            }
        };

        check(args.letter_map());
        check(args.string_map());
    }

    bool check_requirements() const {
        bool met = true;
        for (const auto &pair : options_) {
            const auto &name = pair.first;
            const auto option = pair.second;
            if (option->required() && existing_args_.find(name) == existing_args_.end()) {
                if (cbs_.missing) {
                    cbs_.missing(std::string(name));
                }
                met = false;
            }
        }
        return met;
    }
};

} // namespace argparse
