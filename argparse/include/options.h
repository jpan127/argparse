#pragma once

#include "option.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace argparse {

class Options {
    using OptionTable = Option::OptionTable;
    using MapType = std::unordered_map<std::string, std::shared_ptr<Option>>;

  public:
    using NameLetterVector = std::vector<std::pair<std::string, std::string>>;

    template <typename T>
    ConstPlaceHolder<T> add(Config<T> &&config);

    template <typename T>
    ConstPlaceHolder<std::vector<T>> add_multivalent(Config<T> &&config);

    /// Creates a string for the usage message
    /// \note Positionals are skipped and are handled by the [Parser]
    std::string usage_string() const;

    /// Creates a display string for all the options
    /// Generates a table of the details of every option
    std::string display_string() const;

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
    void add_helper(Config<T> &&config, PlaceholderType &placeholder);
};

} // namespace argparse
