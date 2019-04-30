#pragma once

#include "args.h"
#include "options.h"

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace argparse {
namespace detail {

class Parser {
  public:
    Args parse(const int argc, const char **argv) {
        Args args{};
        std::string last_option;

        for (std::size_t ii = 1; ii < static_cast<std::size_t>(argc); ii++) {
            parse_arg(args, last_option, argv[ii]);
        }

        return args;
    }

  private:
    void parse_arg(Args &args, std::string &last_option, std::string s) {
        // If this is an option, set it as the option for future token
        if (is_option(s)) {
            strip_prefix(s);
            last_option = std::move(s);
            if (last_option.length() == 1) {
                args.create(last_option[0]);
            } else {
                args.create(last_option);
            }
        } else {
            // No last option means an option has not been found yet
            if (last_option.empty()) {
                return;
            }

            // Map the option to this value
            if (last_option.length() == 1) {
                args.insert(last_option[0], std::move(s));
            } else {
                args.insert(last_option, std::move(s));
            }
        }
    }

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
