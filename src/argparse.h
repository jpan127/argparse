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
                printf("Argument invalid : [--%s=%s]\n", name.c_str(), v);
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

        if (cbs_.help) {
            cbs_.help();
        }
    }

    const Args &parse(const int argc, const char **argv) {
        const auto &arg_map = parser_.parse(argc, argv);
        for (const auto &pair : arg_map) {
            const auto &name = pair.first;
            const auto &values = pair.second;
            auto option = (name.length() == 1) ? (options_.get(name[0])) : (options_.get(name));
            if (option) {
                if (option->type() == kBool) {
                    option->set("true");
                    args_.add(option->name(), option);
                } else {
                    if (values.empty()) {
                        if (cbs_.missing) {
                            cbs_.missing(name);
                        }
                    } else {
                        /// @TODO : Only supporting one value right now
                        option->set(values[0]);
                        args_.add(option->name(), option);
                    }
                }
            } else {
                if (cbs_.invalid) {
                    cbs_.invalid(name, values);
                }
            }
        }

        // Check against required arguments
        bool missing_any = false;
        for (const auto &pair : options_) {
            const auto &name = pair.first;
            const auto option = pair.second;
            if (!args_.exists(name) && option->required()) {
                if (cbs_.missing) {
                    cbs_.missing(name);
                }
                missing_any = true;
            }
        }

        if (missing_any) {
            help();
            cbs_.exit();
        }

        const auto arg = args_.get<bool>("help");
        if (arg.has_value()) {
            help();
            cbs_.exit();
        }

        return args_;
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
    Args args_;
    detail::Parser parser_;
};

} // namespace argparse
