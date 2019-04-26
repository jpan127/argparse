#pragma once

#include "args.h"
#include "options.h"

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace argparse {
namespace detail {

class Parser {
  public:
    using ArgMap = std::unordered_map<std::string, std::vector<std::string>>;

    const ArgMap& parse(const int argc, const char **argv) {
        std::string last_option;
        for (std::size_t ii = 1; ii < static_cast<std::size_t>(argc); ii++) {
            // Store into vector
            argv_.emplace_back(argv[ii]);
            // Parse the argument
            parse_arg(ii, last_option, argv[ii]);
        }

        return arg_map_;
    }

    void parse_arg(const std::size_t index, std::string &last_option, std::string s) {
        // If this is an option, set it as the option for future token
        if (is_option(s)) {
            strip_prefix(s);
            last_option = std::move(s);
            arg_map_[last_option];
        } else {
            // No last option means an option has not been found yet
            if (last_option.empty()) {
                positional_args_.insert(index);
                return;
            }

            // Map the option to this value
            arg_map_[last_option].push_back(std::move(s));
        }
    }

    const ArgMap &args() const {
        return arg_map_;
    }

  private:
    ArgMap arg_map_;
    std::vector<std::string> argv_;
    std::unordered_set<std::size_t> positional_args_;

    bool is_option(const std::string &s) {
        assert(s.length() > 0);

        if (s[0] == '-') {
            return true;
        }

        if (s.length() >= 2 && s[0] == '-' && s[1] == '-') {
            return true;
        }

        return false;
    }

    void strip_prefix(std::string &s) {
        assert(s[0] == '-');
        s.erase(0, 1);
        if (s[0] == '-') {
            s.erase(0, 1);
        }
    }
};

} // namespace detail
} // namespace argparse
