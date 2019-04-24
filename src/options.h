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
    void add(const Option::Config &config) {
        options_[config.name] = std::make_shared<Option>(config, T{});
    }

    template <typename T>
    void add(const Option::Config &config, const T &default_value) {
        options_[config.name] = std::make_shared<Option>(config, default_value);
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

    auto begin() {
        return options_.begin();
    }

    auto end() {
        return options_.end();
    }

  private:
    MapType options_{};
};

} // namespace argparse
