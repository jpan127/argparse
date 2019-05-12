#pragma once

#include "std_variant.h"

#include <cassert>
#include <ostream>
#include <sstream>
#include <unordered_set>

namespace argparse {

namespace detail {

template <typename T>
constexpr bool acceptable() {
    if (std::is_same<T, std::string>::value) {
        return true;
    }

    return std::is_fundamental<T>::value;
}

} // namespace detail

class Variant {
  public:
    enum Type {
        kString,
        kUint64,
        kInt64,
        kDouble,
        kFloat,
        kBool,
        kVariantsSize,
    };

    using V = pstd::variant<
        std::string,
        uint64_t,
        int64_t,
        double,
        float,
        bool>;

    static_assert(kVariantsSize == pstd::variant_size_v<V>, "");

    template <typename T, typename X = std::enable_if_t<std::is_same<T, Variant>::value, void>>
    explicit Variant(T &&value) : value_(std::forward<T>(value)) {
    }

    template <typename T>
    explicit Variant(const T &value) : value_(value) {
    }

    friend std::ostream &operator<<(std::ostream &stream, Variant &variant) {
        stream << variant.string();
        return stream;
    }

    static const char *enum_to_str(const Type type) {
        switch (type) {
        case kString : return "std::string";
        case kUint64 : return "uint64_t";
        case kInt64  : return "int64_t";
        case kDouble : return "double";
        case kFloat  : return "float";
        case kBool   : return "bool";
        default      : assert(false);
        }

        return nullptr;
    }

    struct Visitor {
        template <typename T>
        std::string operator()(const T &value) const {
            std::stringstream ss;
            ss << value;
            return ss.str();
        }
    };
    std::string string() const {
        return pstd::visit(Visitor{}, value_);
    }

    template <typename T>
    T &get() {
        return pstd::get<T>(value_);
    }

    template <std::size_t Index>
    auto &get() {
        return pstd::get<Index>(value_);
    }

    template <typename T>
    const T &get() const {
        return pstd::get<T>(value_);
    }

    template <std::size_t Index>
    const auto &get() const {
        return pstd::get<Index>(value_);
    }

    struct hash {
        std::size_t operator()(const argparse::Variant &v) const {
            return std::hash<V>{}(v.value_);
        }
    };

    bool operator==(const argparse::Variant &other) const {
        return value_ == other.value_;
    }

  private:
    V value_;
};

namespace detail {

/// @{ Converts a type to enum
template <typename T> constexpr Variant::Type deduce_variant();
template <> constexpr Variant::Type deduce_variant<std::string>() { return Variant::Type::kString; }
template <> constexpr Variant::Type deduce_variant<uint64_t>() { return Variant::Type::kUint64; }
template <> constexpr Variant::Type deduce_variant<int64_t>() { return Variant::Type::kInt64; }
template <> constexpr Variant::Type deduce_variant<double>() { return Variant::Type::kDouble; }
template <> constexpr Variant::Type deduce_variant<float>() { return Variant::Type::kFloat; }
template <> constexpr Variant::Type deduce_variant<bool>() { return Variant::Type::kBool; }
/// @}

/// Creates a variant given the initial value
template <typename T>
Variant make_variant(const T &default_value) {
    return Variant{default_value};
}

template <typename T>
std::unordered_set<Variant, Variant::hash> make_variants(const std::unordered_set<T> &in) {
    std::unordered_set<Variant, Variant::hash> out;
    for (auto &&value : in) {
        out.insert(make_variant(value));
    }
    return out;
}

} // namespace detail

} // namespace argparse
