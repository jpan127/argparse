#pragma once

#include "std_optional.h"

#include <string>
#include <unordered_set>

namespace argparse {

constexpr char kUnusedChar = 0;

/// Configuration of the option
template <typename T>
struct Config {
    pstd::optional<T> default_value{};
    std::unordered_set<T> allowed_values{};
    std::string name{};        /// Name of the option, multicharacter string
    std::string help{};        /// Optional help message
    bool positional = false;   /// Whether this option is position based or name based
    bool required = false;     /// Should enforce requirement of the option
    char letter = kUnusedChar; /// Character of the option, if != kUnusedChar
};

} // namespace argparse
