#pragma once

#include "std_variant.h"

#include <cassert>
#include <ostream>
#include <sstream>

namespace argparse {

enum Variants {
    kString,
    kUint64,
    kInt64,
    kDouble,
    kFloat,
    kBool,
    kVariantsSize,
};

using V = pre_std::variant<
    std::string,
    uint64_t,
    int64_t,
    double,
    float,
    bool
>;

static_assert(kVariantsSize == pre_std::variant_size_v<V>, "");

inline const char *enum_to_str(const Variants variant_type) {
    switch (variant_type) {
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

/// Convert variant value to string
inline std::string string(const V &variant) {
    auto cb = [](auto &&value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    };

    switch (variant.index()) {
    case kString : return cb(pre_std::get<kString>(variant));
    case kUint64 : return cb(pre_std::get<kUint64>(variant));
    case kInt64  : return cb(pre_std::get<kInt64>(variant));
    case kDouble : return cb(pre_std::get<kDouble>(variant));
    case kFloat  : return cb(pre_std::get<kFloat>(variant));
    case kBool   : return cb(pre_std::get<kBool>(variant));
    default      : assert(false);
    }

    return "";
}

/// Output stream opreator for a variant
inline std::ostream & operator<<(std::ostream &stream, const V &variant) {
    const auto s = string(variant);
    stream << s;
    return stream;
}

} // namespace argparse
