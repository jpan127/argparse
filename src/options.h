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
    ConstPlaceHolder<T> add(const Option::Config &config, const pre_std::optional<T> &default_value,
                            std::unordered_set<T> &&allowed_values = {}) {
        auto placeholder = std::make_shared<PlaceHolderType<T>>();
        options_[config.name] = std::make_shared<Option>(
            placeholder,
            config,
            default_value,
            std::forward<std::unordered_set<T>>(allowed_values)
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
