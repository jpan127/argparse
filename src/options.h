#pragma once

#include "option.h"

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

namespace argparse {

class Options {
    using MapType = std::unordered_map<std::string, std::shared_ptr<Option>>;

  public:
    template <typename T>
    std::shared_ptr<const T> add(const Option::Config &config, const T &default_value) {
        auto value_handle = std::make_shared<T>();
        options_[config.name] = std::make_shared<Option>(value_handle, config, default_value);
        return value_handle;
    }

    void display() const {
        for (const auto &pair : options_) {
            const auto &option = pair.second;
            std::cout << (*option) << std::endl;
        }
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
