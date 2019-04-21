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
    ptr->default_value.emplace<kString>(default_value.data());
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
    case kString : cb(pre_std::get<kString>(variant)); break;
    case kUint64 : cb(pre_std::get<kUint64>(variant)); break;
    case kInt64  : cb(pre_std::get<kInt64>(variant));  break;
    case kDouble : cb(pre_std::get<kDouble>(variant)); break;
    case kFloat  : cb(pre_std::get<kFloat>(variant));  break;
    case kBool   : cb(pre_std::get<kBool>(variant));   break;
    }
}

template <typename OutputType, typename Callback>
OutputType handle_variant(const VariantOption &variant_option, const Callback &cb) {
    switch (variant_option.variant) {
    case kString : return cb(pre_std::get<kString>(variant_option.value));
    case kUint64 : return cb(pre_std::get<kUint64>(variant_option.value));
    case kInt64  : return cb(pre_std::get<kInt64>(variant_option.value));
    case kDouble : return cb(pre_std::get<kDouble>(variant_option.value));
    case kFloat  : return cb(pre_std::get<kFloat>(variant_option.value));
    case kBool   : return cb(pre_std::get<kBool>(variant_option.value));
    }

    assert(false);
}

const char *enum_to_str(const Variants variant_type) {
    switch (variant_type) {
    case kString : return "std::string";
    case kUint64 : return "uint64_t";
    case kInt64  : return "int64_t";
    case kDouble : return "double";
    case kFloat  : return "float";
    case kBool   : return "bool";
    }

    assert(false);
}

template <typename InputType, typename OutputType, typename ... Types>
OutputType type_switch(const InputType value, const std::function<OutputType(const Types &)> & ... cbs) {
    for (auto && cb : {cbs...}) {
        using CallbackType = typename decltype(cb)::argument_type;
        if (std::is_same<InputType, CallbackType>::value) {
            return cb(value);
        }
    }
}

template <typename T>
constexpr bool acceptable() {
    if (std::is_same<T, std::string>::value) {
        return true;
    }

    return std::is_fundamental<T>::value;
}

} // namespace detail
} // namespace argparse
