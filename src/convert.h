#pragma once

#include "std_optional.h"
#include "options_types.h"

#include <string>
#include <memory>
#include <type_traits>
#include <limits>

namespace argparse {
namespace detail {

/// Trait for checking if both types are fundamental types
/// https://en.cppreference.com/w/cpp/language/types
template <typename OutputType, typename InputType>
struct is_safely_castable {
    static constexpr bool value =
        (std::is_fundamental<OutputType>::value  &&
         std::is_fundamental<InputType>::value);
    constexpr bool operator()() {
        return value;
    }
};

/// Inline constexpr's only work in C++17 :(
#if __cplusplus >= 201703L
template <typename OutputType, typename InputType>
inline constexpr bool is_safely_castable_v = is_safely_castable<OutputType, InputType>::value;
#endif

/// Unspecialized conversion helper
template <typename OutputType, typename InputType>
OutputType convert_helper(const InputType &input);

/// Specializations for from std::string
template <> std::string convert_helper(const std::string &input) { return input; }
template <> uint64_t convert_helper(const std::string &input) { return std::stoull(input); }
template <> int64_t convert_helper(const std::string &input) { return std::stoll(input); }
template <> double convert_helper(const std::string &input) { return std::stod(input); }
template <> float convert_helper(const std::string &input) { return std::stof(input); }
template <> bool convert_helper(const std::string &input) { return (input == "true" || input == "True"); }

/// Specializations for to std::string
template <> std::string convert_helper(const uint64_t &input) { return std::to_string(input); }
template <> std::string convert_helper(const int64_t &input) { return std::to_string(input); }
template <> std::string convert_helper(const double &input) { return std::to_string(input); }
template <> std::string convert_helper(const float &input) { return std::to_string(input); }
template <> std::string convert_helper(const bool &input) { return std::to_string(input); }

/// Dispatcher for static_cast-able conversions
template <typename OutputType, typename InputType>
typename std::enable_if<is_safely_castable<OutputType, InputType>::value, OutputType>::type convert(const InputType &input) {
    return static_cast<OutputType>(input);
}

/// Dispatcher for non static_cast-able conversions
template <typename OutputType, typename InputType>
typename std::enable_if<!is_safely_castable<OutputType, InputType>::value, OutputType>::type convert(const InputType &input) {
    return convert_helper<OutputType, InputType>(input);
}

/// Dispatcher for cases from string literals
template <typename OutputType, std::size_t N>
OutputType convert(const char (&input)[N]) {
    return convert_helper<OutputType, std::string>(std::string(input));
}

/// Convert from variant option to output type
template <typename OutputType>
OutputType convert(const std::shared_ptr<VariantOption> option) {
    switch (option->variant) {
    case kString : return detail::convert<OutputType, std::string>(pre_std::get<std::string>(option->value));
    case kUint64 : return detail::convert<OutputType, uint64_t>(pre_std::get<uint64_t>(option->value));
    case kInt64  : return detail::convert<OutputType, int64_t>(pre_std::get<int64_t>(option->value));
    case kDouble : return detail::convert<OutputType, double>(pre_std::get<double>(option->value));
    case kFloat  : return detail::convert<OutputType, float>(pre_std::get<float>(option->value));
    case kBool   : return detail::convert<OutputType, bool>(pre_std::get<bool>(option->value));
    }
}

} // namespace detail
} // namespace argparse
