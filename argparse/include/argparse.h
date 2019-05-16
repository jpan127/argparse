#pragma once

#include "args.h"
#include "config.h"
#include "placeholder.h"
#include "std_optional.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace argparse {

/// Forward Declarations
class Options;
namespace detail {
class Parser;
} // namespace detail

class Parser {
  public:
    struct Callbacks {
        std::function<void()> exit;
        std::function<void()> help;
        std::function<void(const std::string &)> missing;
        std::function<void(const std::string &, const std::vector<std::string> &)> invalid;
    };

    /// Constructor
    explicit Parser(std::string name = "", std::string help = "");

    /// Destructor
    ~Parser();

    /// No copy, move, or assignment
    Parser(const Parser &other) = delete;
    Parser(Parser &&other) = delete;
    Parser &operator=(const Parser &other) = delete;
    Parser &operator=(Parser &&other) = delete;

    template <typename T>
    ConstPlaceHolder<std::vector<T>> add_multivalent(Config<T> config);

    template <typename T>
    ConstPlaceHolder<T> add(Config<T> config);

    /// Add overload to specify individual arguments
    /// Non const inputs are moved
    template <typename T>
    ConstPlaceHolder<T> add(std::string name, // NOLINT(performance-unnecessary-value-param)
                            std::string help, // NOLINT(performance-unnecessary-value-param)
                            const char letter = kUnusedChar,
                            const bool required = false,
                            pstd::optional<T> default_value = T{},
                            std::unordered_set<T> allowed_values = {});

    template <typename T>
    ConstPlaceHolder<T> add_leading_positional(Config<T> config);

    /// For each valid callback, sets it
    /// Ignores null callback objects so existing callback will not be overwritten
    void set_callbacks(Callbacks &&cbs);

    void help() const;

    /// Parse arguments
    /// \return Remaining, non-parsed arguments
    const std::vector<std::string> &parse(const int argc, const char **argv);

    std::unordered_map<std::string, Parser> &add_subparser(std::string &&group,
                                                           std::unordered_set<std::string> &&allowed_values);

    const std::string &subparser() const {
        return selected_subparser_;
    }

  private:
    const std::string name_ = "<PROGRAM_NAME>";
    const std::string help_ = "<HELP_MESSAGE>";

    Callbacks cbs_{};
    std::unique_ptr<Options> options_;
    std::unique_ptr<detail::Parser> parser_;
    std::vector<std::string> remaining_args_;
    std::unordered_set<std::string> existing_args_;

    /// Positional arguments that go before other arguments
    std::vector<std::string> positionals_;

    /// Map of subparser name to subparser
    pstd::optional<std::unordered_map<std::string, Parser>> subparser_;

    /// Name of the subparser group
    pstd::optional<std::string> subparser_group_;

    /// Chosen subparser from the subparser group
    std::string selected_subparser_;

    template <typename T>
    void validate(Config<T> &config);

    /// Parse arguments
    /// \param pop_first To remove the first argument or not
    /// \return          Remaining, non-parsed arguments
    const std::vector<std::string> &parse(const int argc, const char **argv, const bool pop_first);

    void cross_check(Args &&args);

    bool check_requirements() const;

    /// Set the default callbacks into [cbs_]
    void set_default_callbacks();
};

} // namespace argparse
