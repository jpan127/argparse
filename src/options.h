#pragma once

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
    template <typename T>
    ConstPlaceHolder<T> add(Option::Config<T> &&config) {
        // Need to copy name as it is used on the LHS by operator[], but is moved on the RHS
        // Order of evaluation seems to be undefined
        const auto name = config.name;
        auto placeholder = std::make_shared<PlaceHolderType<T>>();
        options_[name] = std::make_shared<Option>(
            placeholder,
            std::forward<Option::Config<T>>(config)
        );
        return placeholder;
    }

    template <typename T>
    ConstPlaceHolder<std::vector<T>> add_multivalent(Option::Config<T> &&config) {
        // Need to copy name as it is used on the LHS by operator[], but is moved on the RHS
        // Order of evaluation seems to be undefined
        const auto name = config.name;
        auto placeholder = std::make_shared<PlaceHolderType<std::vector<T>>>();
        options_[name] = std::make_shared<Option>(
            placeholder,
            std::forward<Option::Config<T>>(config)
        );
        return placeholder;
    }

    void display() const {
        OptionTable table({{"Name", "Type", "Default", "Help", "Allowed Values"}});

        for (const auto &pair : options_) {
            const auto &option = pair.second;
            table.add_row(option->to_string());
        }

        std::cout << table.display();
    }

    std::shared_ptr<Option> get(const std::string &name) {
        const auto iterator = options_.find(name);
        if (iterator != options_.end()) {
            return iterator->second;
        }

        return nullptr;
    }

    std::shared_ptr<Option> get(const char letter) {
        for (const auto &pair : options_) {
            const auto option = pair.second;
            if (option->letter() == letter) {
                return option;
            }
        }

        return nullptr;
    }

    auto begin() const {
        return options_.begin();
    }

    auto end() const {
        return options_.end();
    }

  private:
    MapType options_{};
};

} // namespace argparse
