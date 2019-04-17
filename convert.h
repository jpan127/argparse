#pragma once

#include <string>
#include <memory>

namespace argparse {
namespace detail {

template <typename T>
T convert(const std::string &s);

template <>
std::string convert<std::string>(const std::string &s) {
    return s;
}

template <>
uint64_t convert<uint64_t>(const std::string &s) {
    return std::stoi(s);
}

template <>
int64_t convert<int64_t>(const std::string &s) {
    return std::stoi(s);
}

template <>
double convert<double>(const std::string &s) {
    return std::stod(s);
}

template <>
float convert<float>(const std::string &s) {
    return std::stof(s);
}

template <>
bool convert<bool>(const std::string &s) {
    if (s == "true" || s == "True") {
        return true;
    }

    return false;
}

} // namespace detail
} // namespace argparse
