#pragma once

#include "tl/optional.hpp"

/// Pre C++17 std::optional does not exist
/// This library uses tl::optional as a replacement [https://github.com/TartanLlama/optional]

/// Set the tl namespace to a name that denotes its relation to the standard
namespace pstd {
using namespace tl;
} // namespace pstd
