#pragma once

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

using OptionsVariant = mpark::variant<
    std::string,
    uint64_t,
    int64_t,
    double,
    float,
    bool
>;

static_assert(kVariantsSize == mpark::variant_size_v<OptionsVariant>);

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
    bool has_default = false;
};

} // namespace argparse
