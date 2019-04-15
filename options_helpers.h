#pragma once

#include <memory>

#include "options_types.h"

namespace argparse {
namespace detail {

template <typename T>
std::shared_ptr<VariantOption> add(const Option &options, const T &default_value);

template <>
std::shared_ptr<VariantOption> add(const Option &options, const std::string &default_value) {
    auto ptr = std::make_shared<VariantOption>();
    ptr->opt = std::move(options);
    ptr->variant = kString;
    ptr->default_value.emplace<kString>(default_value);
    ptr->has_default = true;
    return ptr;
}

template <>
std::shared_ptr<VariantOption> add(const Option &options, const uint64_t &default_value) {
    auto ptr = std::make_shared<VariantOption>();
    ptr->opt = std::move(options);
    ptr->variant = kUint64;
    ptr->default_value.emplace<kUint64>(default_value);
    ptr->has_default = true;
    return ptr;
}

template <>
std::shared_ptr<VariantOption> add(const Option &options, const int64_t &default_value) {
    auto ptr = std::make_shared<VariantOption>();
    ptr->opt = std::move(options);
    ptr->variant = kInt64;
    ptr->default_value.emplace<kInt64>(default_value);
    ptr->has_default = true;
    return ptr;
}

template <>
std::shared_ptr<VariantOption> add(const Option &options, const double &default_value) {
    auto ptr = std::make_shared<VariantOption>();
    ptr->opt = std::move(options);
    ptr->variant = kDouble;
    ptr->default_value.emplace<kDouble>(default_value);
    ptr->has_default = true;
    return ptr;
}

template <>
std::shared_ptr<VariantOption> add(const Option &options, const float &default_value) {
    auto ptr = std::make_shared<VariantOption>();
    ptr->opt = std::move(options);
    ptr->variant = kFloat;
    ptr->default_value.emplace<kFloat>(default_value);
    ptr->has_default = true;
    return ptr;
}

template <>
std::shared_ptr<VariantOption> add(const Option &options, const bool &default_value) {
    auto ptr = std::make_shared<VariantOption>();
    ptr->opt = std::move(options);
    ptr->variant = kBool;
    ptr->default_value.emplace<kBool>(default_value);
    ptr->has_default = true;
    return ptr;
}

template <typename Callback>
void handle_variant(const OptionsVariant &variant, const Variants variant_type, const Callback &cb) {
    switch (variant_type) {
    case kString : cb(mpark::get<kString>(variant)); break;
    case kUint64 : cb(mpark::get<kUint64>(variant)); break;
    case kInt64  : cb(mpark::get<kInt64>(variant));  break;
    case kDouble : cb(mpark::get<kDouble>(variant)); break;
    case kFloat  : cb(mpark::get<kFloat>(variant));  break;
    case kBool   : cb(mpark::get<kBool>(variant));   break;
    }
}

} // namespace detail
} // namespace argparse
