#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <cassert>

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
            if (option->has_default) {
                std::cout << " (default=";
                detail::handle_variant(option->default_value, option->variant, [](const auto &v) {
                    std::cout << v;
                });
                std::cout << ")";
            }
            std::cout << "[Help=" << option->opt.help << "]" << std::endl;
        }
    }

    // /// @TODO : Make this string_view
    // bool exists(const std::string &name) {
    //     return (options_.find(name) != options_.end());
    // }

    // template <typename T>
    // std::shared_ptr<T> get(const std::string &name) {
    //     const auto iterator = options_.find(name);
    //     if (iterator != options_.end()) {
    //         const auto voidptr = iterator->second;
    //         const auto baseptr = std::static_pointer_cast<OptionBase>(voidptr);
    //         return std::static_pointer_cast<Option<T>>(baseptr);
    //     }

    //     return nullptr;
    // }

    // auto operator->() {

    // }

  private:
    std::map<std::string, std::shared_ptr<VariantOption>> options_{};
};

} // namespace argparse
