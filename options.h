#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <cassert>
#include <typeinfo>

#include "variant.hpp"
#include "options_helpers.h"

namespace argparse {

class Options {
  public:
    template <typename T>
    void add(const Option &options) {
        auto ptr = std::make_shared<VariantOption>();
        ptr->opt = std::move(options);
        options_[options.name] = ptr;
    }

    template <typename T>
    void add(const Option &options, const T &default_value) {
        options_[options.name] = detail::add<T>(options, default_value);
    }

    void display() const {
        const std::string kPrefix = "  - ";
        for (const auto &pair : options_) {
            const auto &name = pair.first;
            const auto &option = pair.second;
            std::cout << kPrefix << name;
            std::cout << "[" << detail::convert(option->variant) << "] ";
            if (option->has_default) {
                std::cout << "(default=";
                detail::handle_variant(option->default_value, option->variant, [](const auto &v) {
                    std::cout << v;
                });
                std::cout << ")";
            }
            if (!option->opt.help.empty()) {
                std::cout << "[Help=" << option->opt.help << "]";
            }
            std::cout << std::endl;
        }
    }

    std::shared_ptr<VariantOption> get(const std::string &name) {
        const auto iterator = options_.find(name);
        if (iterator != options_.end()) {
            return iterator->second;
        }

        return nullptr;
    }

  private:
    std::map<std::string, std::shared_ptr<VariantOption>> options_{};
};

} // namespace argparse
