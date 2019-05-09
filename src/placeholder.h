#pragma once

#include "std_optional.h"

#include <memory>

/// Optional value
template <typename T>
using PlaceHolderType = pstd::optional<T>;

/// Pointer to optional value
template <typename T>
using PlaceHolder = std::shared_ptr<PlaceHolderType<T>>;

/// Pointer to const optional value
template <typename T>
using ConstPlaceHolder = std::shared_ptr<const PlaceHolderType<T>>;
