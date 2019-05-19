#pragma once

#include "option.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace argparse {

/// Encapsulates a set of options
class Options {
    using OptionTable = Option::OptionTable;
    using MapType = std::unordered_map<std::string, std::shared_ptr<Option>>;

  public:
    using NameLetterVector = std::vector<std::pair<std::string, std::string>>;

    /// Add an option that can have a single value
    /// \param config Configuration for the option
    /// \param positional
    /// \param position
    template <typename T>
    ConstPlaceHolder<T> add(Config<T> &&config, const pstd::optional<std::size_t> position = {});

    /// Add an option that can have multiple values
    /// \param config Configuration for the option
    template <typename T>
    ConstPlaceHolder<std::vector<T>> add_multivalent(Config<T> &&config);

    /// Creates a string for the usage message
    /// \note Positionals are skipped and are handled by the [Parser]
    std::string usage_string() const;

    /// Creates a display string for all the options
    /// Generates a table of the details of every option
    std::string display_string() const;

    /// Searches for an option by name
    /// \param name Name of the option
    /// \return     A pointer to the option if found, otherwise nullptr
    std::shared_ptr<Option> get(const std::string &name);

    /// Check the input set of arguments with the set of required arguments
    /// \return The set of arguments that were required but non existing
    NameLetterVector check_requirements(const std::unordered_set<std::string> &existing_args) const;

  private:
    /// Map of registered options
    MapType options_{};

    /// List of options that are required
    NameLetterVector required_options_{};

    /// Helper for registering an option with a configuration
    template <typename T, typename PlaceholderType>
    void add_helper(Config<T> &&config, PlaceholderType &placeholder, const pstd::optional<std::size_t> position = {});
};

} // namespace argparse
