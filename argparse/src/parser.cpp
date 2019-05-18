#include "parser.h"
#include "options.h"

#include <cassert>

namespace argparse {
namespace detail {

Args Parser::parse(const int argc, const char **argv) {
    Args args{};
    std::string last_option;
    bool is_splitted_args = false;

    for (std::size_t ii = 0; ii < static_cast<std::size_t>(argc); ii++) {
        parse_arg(args, last_option, argv[ii], is_splitted_args);
    }

    return args;
}

void Parser::parse_arg(Args &args, std::string &last_option, std::string &&s, bool &is_splitted_args) {
    constexpr char kSplitter[] = "--";

    if (is_splitted_args) {
        // Rest of arguments go under this category
        args.insert(kRemainingArgsKey, std::move(s));
    } else if (s == kSplitter) {
        is_splitted_args = true;
    } else if (is_option(s)) {
        // If this is an option, set it as the option for future token
        strip_prefix(s);
        last_option = std::move(s);
        args.create(last_option);
    } else {
        // No last option means an option has not been found yet
        // It is assumed to be a positional argument
        if (last_option.empty()) {
            positional_args_.push_back(std::move(s));
            return;
        }

        // Map the option to this value
        args.insert(last_option, std::move(s));
    }
}

bool Parser::is_option(const std::string &s) {
    assert(s.length() > 0);

    if (s[0] == '-') {
        // Check for signed integers
        if (s.length() > 1) {
            if (0 != std::isdigit(s[1])) {
                return false;
            }
        }
        return true;
    }

    return (s.length() >= 2 && s[0] == '-' && s[1] == '-');
}

void Parser::strip_prefix(std::string &s) {
    assert(s[0] == '-');
    s.erase(0, 1);
    if (s[0] == '-') {
        s.erase(0, 1);
    }
}

} // namespace detail
} // namespace argparse
