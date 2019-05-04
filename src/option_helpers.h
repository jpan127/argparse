#pragma once

#include <memory>
#include <unordered_set>

#include "option_types.h"

namespace argparse {
namespace detail {

/// @{ Converts a type to enum
template <typename T> constexpr Variants deduce_variant();
template <> constexpr Variants deduce_variant<std::string>() { return kString; }
template <> constexpr Variants deduce_variant<uint64_t>() { return kUint64; }
template <> constexpr Variants deduce_variant<int64_t>() { return kInt64; }
template <> constexpr Variants deduce_variant<double>() { return kDouble; }
template <> constexpr Variants deduce_variant<float>() { return kFloat; }
template <> constexpr Variants deduce_variant<bool>() { return kBool; }
/// @}

/// Creates a variant given the initial value
template <typename T>
V make_variant(const T &default_value) {
    return V{default_value};
}

template <typename T>
std::unordered_set<V> make_variants(std::unordered_set<T> &&in) {
    std::unordered_set<V> out;
    for (auto &&value : in) {
        out.insert(make_variant(value));
    }
    return out;
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
