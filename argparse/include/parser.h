#pragma once

#include "args.h"

#include <string>
#include <vector>

namespace argparse {
namespace detail {

class Parser {
  public:
    /// Map key string for the remaining / splitted arguments
    static constexpr const char * const kRemainingArgsKey = "_remaining";

    /// Parses the programs input arguments
    /// \return The parsed arguments in a map-like object
    Args parse(const int argc, const char **argv);

    /// \return The parsed positional arguments
    const std::vector<std::string> &positional_args() const;

  private:
    /// Positional arguments are the arguments that are before other options
    std::vector<std::string> positional_args_;

    /// Parse a single argument
    /// \param args             Map of arguments currently parsed
    /// \param last_option      The last option that the current value belongs to
    /// \param s                The value of the argument
    /// \param is_splitted_args If the following arguments are part of the splitted arguments set
    void parse_arg(Args &args, std::string &last_option, std::string &&s, bool &is_splitted_args);

    /// Checks if the string is an option or not by checking for 1-2 hyphens '-'
    bool is_option(const std::string &s);

    /// Strips the prefixing hyphens
    void strip_prefix(std::string &s);
};

} // namespace detail
} // namespace argparse
