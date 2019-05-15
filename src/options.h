#pragma once

#include "exceptions.h"
#include "option.h"

#include <iostream>
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
    ConstPlaceHolder<T> add(Option::Config<T> &&config) {
        auto placeholder = std::make_shared<PlaceHolderType<T>>();
        add_helper<T>(std::move(config), placeholder);
        return placeholder;
    }

    template <typename T>
    ConstPlaceHolder<std::vector<T>> add_multivalent(Option::Config<T> &&config) {
        auto placeholder = std::make_shared<PlaceHolderType<std::vector<T>>>();
        add_helper<T>(std::move(config), placeholder);
        return placeholder;
    }

    /// Creates a string for the usage message
    /// \note Positionals are skipped and are handled by the [Parser]
    std::string usage_string() const {
        auto usage_template = [](const auto &name) { return "[--" + name + "]"; };

        std::stringstream ss;
        for (const auto &pair : options_) {
            const auto &name = pair.first;
            const auto &option = pair.second;
            if (!option->positional()) {
                ss << usage_template(name) << " ";
            }
        }

        return ss.str();
    }

    /// Creates a display string for all the options
    /// Generates a table of the details of every option
    std::string display_string() const {
        OptionTable table({{"Required", "Name", "Type", "Default", "Help", "Allowed Values"}});

        for (const auto &pair : options_) {
            const auto &option = pair.second;
            table.add_row(option->to_string());
        }

        return table.display();
    }

    std::shared_ptr<Option> get(const std::string &name) {
        const auto iterator = options_.find(name);
        if (iterator != options_.end()) {
            return iterator->second;
        }

        return nullptr;
    }

    /// @{ Iterators for the map member
    auto begin() const { return options_.begin(); }
    auto end() const { return options_.end(); }
    /// @}

    /// Check the input set of arguments with the set of required arguments
    /// \return The set of arguments that were required but non existing
    NameLetterVector check_requirements(const std::unordered_set<std::string> &existing_args) const {
        NameLetterVector missing{};
        for (auto &&pair : required_options_) {
            const auto &name = pair.first;
            const auto &letter = pair.second;
            const bool name_exists = existing_args.find(name) != existing_args.end();
            const bool letter_exists = existing_args.find(letter) != existing_args.end();
            if (!name_exists && !letter_exists) {
                missing.push_back(pair);
            }
        }

        return missing;
    }

  private:
    /// Map of registered options
    MapType options_{};

    /// List of options that are required
    NameLetterVector required_options_{};

    /// Helper for registering an option with a configuration
    template <typename T, typename PlaceholderType>
    void add_helper(Option::Config<T> &&config, PlaceholderType &placeholder) {
        // Need to copy name as it is used on the LHS by operator[], but is moved on the RHS
        // Order of evaluation seems to be undefined
        const auto name = config.name;
        auto option = std::make_shared<Option>(placeholder, std::forward<Option::Config<T>>(config));
        options_[name] = option;

        // If the letter is valid and different from name, also add it to the map
        const auto letter_to_str = std::string(1, config.letter);
        const bool should_use_letter = (config.letter != Option::kUnusedChar) &&
                                       (config.name   != letter_to_str);
        if (should_use_letter) {
            options_[letter_to_str] = option;
        }

        if (config.required) {
            required_options_.push_back({name, letter_to_str});
        }
    }
};

} // namespace argparse
