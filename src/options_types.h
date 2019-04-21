#pragma once

#include "std_variant.h"

#include <memory>

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

template <Variants>
struct VariantType;

template <> struct VariantType<Variants::kString> { using Type = std::string; };
template <> struct VariantType<Variants::kUint64> { using Type = uint64_t; };
template <> struct VariantType<Variants::kInt64> { using Type = int64_t; };
template <> struct VariantType<Variants::kDouble> { using Type = double; };
template <> struct VariantType<Variants::kFloat> { using Type = float; };
template <> struct VariantType<Variants::kBool> { using Type = bool; };

using OptionsVariant = pre_std::variant<
    std::string,
    uint64_t,
    int64_t,
    double,
    float,
    bool
>;

static_assert(kVariantsSize == pre_std::variant_size_v<OptionsVariant>);

struct Option {
    std::string name{};
    std::string help{};
    bool required = false;
    char letter{};
};

struct VariantOption {
    Option opt{};
    Variants variant{};
    OptionsVariant default_value{};
    OptionsVariant value{};
    bool has_default = false;
};

} // namespace argparse
