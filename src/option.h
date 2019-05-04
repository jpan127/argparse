#pragma once

#include "convert.h"
#include "option_helpers.h"
#include "placeholder.h"
#include "std_variant.h"
#include "table.h"

#include <cassert>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_set>

namespace argparse {

class Option {
  public:
    static constexpr std::size_t kTableSize = 5;
    using OptionTable = Table<kTableSize, Alignment::Center>;

    /// Configuration of the option
    struct Config {
        static constexpr char kUnusedChar = 0;
        std::string name{};
        std::string help{};
        bool required = false;
        char letter{};
    };

    /// Constructor
    template <typename T>
    Option(const PlaceHolder<T> &placeholder, Config config, const pre_std::optional<T> &default_value,
           std::unordered_set<T> &&allowed_values)
        : config_(std::move(config)),
          type_(detail::deduce_variant<T>()),
          default_value_(determine_default_value(default_value)),
          allowed_values_(detail::make_variants(std::forward<std::unordered_set<T>>(allowed_values))),
          placeholder_(placeholder) {

        assert(placeholder_);

        // Set default value
        if (default_value.has_value()) {
            auto typed_ptr = std::static_pointer_cast<PlaceHolderType<T>>(placeholder_);
            *typed_ptr = default_value.value();
        }
    }

    OptionTable::Row to_string() const {
        // Stringify default value
        std::stringstream default_value_str;
        if (default_value_.has_value()) {
            default_value_str << default_value_.value();
        }

        std::string allowed_values_str;
        if (!allowed_values_.empty()) {
            std::stringstream ss;
            ss << " ";
            constexpr char kSeparator[] = " , ";
            for (const auto &v : allowed_values_) {
                ss << v << kSeparator;
            }

            // Erase last separator
            allowed_values_str = ss.str();
            allowed_values_str.erase(allowed_values_str.length() - strlen(kSeparator));
            allowed_values_str += " ";
        }

        return {{
            config_.name,
            enum_to_str(type_),
            default_value_str.str(),
            config_.help,
            allowed_values_str,
        }};
    }

    /// \returns True if set successfully, false if not (value is not allowed)
    bool set(const std::string &s) {
        switch (type_) {
        case kString : return set_helper<std::string>(s); break;
        case kUint64 : return set_helper<uint64_t>(s); break;
        case kInt64  : return set_helper<int64_t>(s); break;
        case kDouble : return set_helper<double>(s); break;
        case kFloat  : return set_helper<float>(s); break;
        case kBool   : return set_helper<bool>(s); break;
        default      : assert(false);
        }

        return false;
    }

    template <typename OutputType>
    OutputType convert() const {
        switch (type_) {
        case kString : return detail::convert<OutputType, std::string>(pre_std::get<std::string>(value_));
        case kUint64 : return detail::convert<OutputType, uint64_t>(pre_std::get<uint64_t>(value_));
        case kInt64  : return detail::convert<OutputType, int64_t>(pre_std::get<int64_t>(value_));
        case kDouble : return detail::convert<OutputType, double>(pre_std::get<double>(value_));
        case kFloat  : return detail::convert<OutputType, float>(pre_std::get<float>(value_));
        case kBool   : return detail::convert<OutputType, bool>(pre_std::get<bool>(value_));
        default      : assert(false);
        }

        return OutputType{};
    }

    const std::string &name() const {
        return config_.name;
    }

    char letter() const {
        return config_.letter;
    }

    Variants type() const {
        return type_;
    }

    bool required() const {
        return config_.required;
    }

  private:
    const Config config_;
    const Variants type_;
    const pre_std::optional<V> default_value_;
    const std::unordered_set<V> allowed_values_;

    /// Current value of the option
    V value_;

    /// Handle to value to be populated
    std::shared_ptr<void> placeholder_;

    /// Determines the value of [default_value_]
    template <typename T>
    pre_std::optional<V> determine_default_value(const pre_std::optional<T> &default_value) {
        if (default_value.has_value()) {
            return detail::make_variant(default_value.value());
        }

        return {};
    }

    /// \returns True if set successfully, false if not (value is not allowed)
    template <typename T>
    bool set_helper(const std::string &s) {
        const auto value = detail::convert_helper<T>(s);

        // Check
        if (!allowed_values_.empty()) {
            bool matched = false;
            for (const auto &v : allowed_values_) {
                const auto &allowed_value = pre_std::get<T>(v);
                if (value == allowed_value) {
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                return false;
            }
        }

        value_.emplace<T>(value);
        auto typed_ptr = std::static_pointer_cast<PlaceHolderType<T>>(placeholder_);
        *typed_ptr = value;

        return true;
    }
};

} // namespace argparse
