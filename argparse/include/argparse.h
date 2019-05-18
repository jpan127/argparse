#pragma once

#include "config.h"
#include "placeholder.h"
#include "std_optional.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace argparse {

/// Forward Declarations
class Args;
class Options;
namespace detail {
class Parser;
} // namespace detail

/// Main interface to the parsing library
/// Quick Start:
/// There are 3 main methods to add options, each of these methods returns a placeholder
/// The placeholder is a [shared_ptr], which will always be a valid pointer, so the value can be modified internally at parsing
/// The placeholder is also a [shared_ptr<optional>] so the value can be optionally present
///     - add
///     - add_leading_positional
///     - add_multivalent
/// After all options are registered, the library needs to parse it with
///     - parse
class Parser {
  public:
    /// Customizable callbacks for exceptional situations
    struct Callbacks {
        std::function<void()> exit;
        std::function<void()> help;
        std::function<void(const std::string &)> missing;
        std::function<void(const std::string &, const std::vector<std::string> &)> invalid;
    };

    /// Constructor
    /// \param name Name of the program
    /// \param help Help message / description of the program
    explicit Parser(std::string name = "", std::string help = "");

    /// Destructor
    ~Parser();

    /// No copy, move, or assignment
    Parser(const Parser &other) = delete;
    Parser(Parser &&other) = delete;
    Parser &operator=(const Parser &other) = delete;
    Parser &operator=(Parser &&other) = delete;

    /// Add an option that can have multiple values
    /// \param config Configuration for the option
    template <typename T>
    ConstPlaceHolder<std::vector<T>> add_multivalent(Config<T> config);

    /// Add an option that can have a single value
    /// \param config Configuration for the option
    template <typename T>
    ConstPlaceHolder<T> add(Config<T> config);

    /// Add a positional argument that must come before other non-positional arguments
    /// Positional arguments are expected in the order this function is called
    /// \param config Configuration for the option
    template <typename T>
    ConstPlaceHolder<T> add_leading_positional(Config<T> config);

    /// Add overload to specify individual arguments
    /// Non const inputs are moved
    /// All parameters are composed into a Config<T>
    template <typename T>
    ConstPlaceHolder<T> add(std::string name, // NOLINT(performance-unnecessary-value-param)
                            std::string help, // NOLINT(performance-unnecessary-value-param)
                            const char letter = kUnusedChar,
                            const bool required = false,
                            pstd::optional<T> default_value = T{},
                            std::unordered_set<T> allowed_values = {});

    /// Stores each valid callback
    /// Ignores null callback objects so existing callback will not be overwritten
    void set_callbacks(Callbacks &&cbs);

    /// Prints the help message
    void help() const;

    /// Parse arguments
    /// \return Remaining arguments that come after a "--"
    const std::vector<std::string> &parse(const int argc, const char **argv);

    /// Creates subparser(s) from a set of allowed values
    /// The subparser(s) are just like another [Parser] object, except each's options are separate from other subparser's options
    /// The idea behind a subparser is for different options to be handled for a specific group
    /// \param group          The name of the group of subparsers
    /// \param allowed_values The set of allowed values that the group can be
    /// \return               A map of group value to the subparser
    /// \note                 The map is non-const so the subparsers can be mutated, but only the valid values contain valid subparsers
    /// \code{.cpp}
    ///   auto &subparsers = parser.add_subparser("mode", {"a", "b", "c"});
    ///   auto &a = subparsers["a"];
    ///   auto &b = subparsers["a"];
    ///   auto &c = subparsers["a"];
    ///   a.add({.name = "option_only_for_a"});
    ///   b.add({.name = "option_only_for_b"});
    ///   c.add({.name = "option_only_for_c"});
    /// \endcode
    std::unordered_map<std::string, Parser> &add_subparser(std::string &&group,
                                                           std::unordered_set<std::string> &&allowed_values);

    /// Return the key of which subparser is currently chosen
    const std::string &subparser() const {
        return selected_subparser_;
    }

  private:
    const std::string name_ = "<PROGRAM_NAME>"; /// Name of the program
    const std::string help_ = "<HELP_MESSAGE>"; /// Help message / description of the program

    /// Customized callbacks
    Callbacks cbs_{};

    /// All the registered options
    std::unique_ptr<Options> options_;

    /// Lower level parser object
    std::unique_ptr<detail::Parser> parser_;

    /// Remaining arguments after the "--" splitter
    std::vector<std::string> remaining_args_;

    /// Set of arguments that were parsed
    std::unordered_set<std::string> existing_args_;

    /// Positional arguments that go before other arguments
    std::vector<std::string> positionals_;

    /// Map of subparser name to subparser
    pstd::optional<std::unordered_map<std::string, Parser>> subparser_;

    /// Name of the subparser group
    pstd::optional<std::string> subparser_group_;

    /// Chosen subparser from the subparser group
    std::string selected_subparser_;

    /// Validates the configuration name / letter is correct
    /// If the name is empty then the name becomes equal to the letter
    template <typename T>
    void validate(Config<T> &config);

    /// Parse arguments
    /// \param pop_first To remove the first argument or not
    /// \return          Remaining, non-parsed arguments
    const std::vector<std::string> &parse(const int argc, const char **argv, const bool pop_first);

    /// Checks the parsed arguments against the registered options
    void cross_check(Args &&args);

    /// Checks if all the required options have been provided
    bool check_requirements() const;

    /// Set the default callbacks into [cbs_]
    void set_default_callbacks();
};

} // namespace argparse
