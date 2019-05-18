#pragma once

#include "std_optional.h"

#include <memory>

/// These are types for a "placeholder" concept
/// The placeholder is a [shared_ptr], which will always be a valid pointer, so the value can be modified internally at parsing
/// The placeholder is also a [shared_ptr<optional>] so the value can be optionally present

/// Optional value
template <typename T>
using PlaceHolderType = pstd::optional<T>;

/// Pointer to optional value
template <typename T>
using PlaceHolder = std::shared_ptr<PlaceHolderType<T>>;

/// Pointer to const optional value
template <typename T>
using ConstPlaceHolder = std::shared_ptr<const PlaceHolderType<T>>;
