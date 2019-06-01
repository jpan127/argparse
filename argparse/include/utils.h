#pragma once

#include <sstream>
#include <string>
#include <vector>

/// For each string in the vector, split into separate strings by comma
/// For example:
///     "a,b,c" => {"a", "b", "c"}
inline void split_values(std::vector<std::string> &values) {
    constexpr char kDelimiter = ',';

    std::vector<std::string> additional;

    for (auto &value : values) {
        // Split value if possible
        std::vector<std::string> splitted_values;
        std::string splitted_value;
        std::istringstream iss(value);
        while (getline(iss, splitted_value, kDelimiter)) {
            if (!splitted_value.empty()) {
                splitted_values.push_back(std::move(splitted_value));
            }
        }

        std::move(splitted_values.begin(), splitted_values.end(), std::back_inserter(additional));
    }

    // Move other vector into original vector
    values.clear();
    std::move(additional.begin(), additional.end(), std::back_inserter(values));
}
