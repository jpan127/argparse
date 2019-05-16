#pragma once

#include "std_optional.h"

#include <limits>
#include <memory>
#include <string>
#include <type_traits>

namespace argparse {
namespace detail {

/// Unspecialized conversion helper
template <typename OutputType, typename InputType>
OutputType convert_helper(const InputType &input);

/// Specializations for from std::string
template <> inline std::string convert_helper(const std::string &input) { return input; }
template <> inline double convert_helper(const std::string &input) { return std::stod(input); }
template <> inline float convert_helper(const std::string &input) { return std::stof(input); }
template <> inline uint64_t convert_helper(const std::string &input) { return std::stoull(input); }
template <> inline int64_t convert_helper(const std::string &input) { return std::stoll(input); }
template <> inline uint32_t convert_helper(const std::string &input) { return std::stoul(input); }
template <> inline int32_t convert_helper(const std::string &input) { return std::stol(input); }
template <> inline uint16_t convert_helper(const std::string &input) { return std::stoul(input); }
template <> inline int16_t convert_helper(const std::string &input) { return std::stol(input); }
template <> inline uint8_t convert_helper(const std::string &input) { return std::stoul(input); }
template <> inline int8_t convert_helper(const std::string &input) { return std::stol(input); }
template <> inline bool convert_helper(const std::string &input) { return (input == "true" || input == "True"); }
template <> inline char convert_helper(const std::string &input) { return input.empty() ? '\0' : input[0]; }

template <> inline std::string convert_helper(const double &input) { return std::to_string(input); }
template <> inline std::string convert_helper(const float &input) { return std::to_string(input); }
template <> inline std::string convert_helper(const uint64_t &input) { return std::to_string(input); }
template <> inline std::string convert_helper(const int64_t &input) { return std::to_string(input); }
template <> inline std::string convert_helper(const uint32_t &input) { return std::to_string(input); }
template <> inline std::string convert_helper(const int32_t &input) { return std::to_string(input); }
template <> inline std::string convert_helper(const uint16_t &input) { return std::to_string(input); }
template <> inline std::string convert_helper(const int16_t &input) { return std::to_string(input); }
template <> inline std::string convert_helper(const uint8_t &input) { return std::to_string(input); }
template <> inline std::string convert_helper(const int8_t &input) { return std::to_string(input); }
template <> inline std::string convert_helper(const bool &input) { return (input) ? "true" : "false"; }
template <> inline std::string convert_helper(const char &input) { return std::string(1, input); }

} // namespace detail
} // namespace argparse
