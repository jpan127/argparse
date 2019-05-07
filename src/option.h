#pragma once

#include "convert.h"
#include "placeholder.h"
#include "table.h"
#include "variant.h"

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
        char letter = kUnusedChar;
    };

    /// Single Constructor
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

    /// Multivalent Constructor
    template <typename T>
    Option(const PlaceHolder<std::vector<T>> &placeholder, Config config, const pre_std::optional<T> &default_value,
           std::unordered_set<T> &&allowed_values)
        : config_(std::move(config)),
          type_(detail::deduce_variant<T>()),
          default_value_(determine_default_value(default_value)),
          allowed_values_(detail::make_variants(std::forward<std::unordered_set<T>>(allowed_values))),
          multivalent_(true),
          placeholder_(placeholder) {

        assert(placeholder_);

        // Set default value
        if (default_value.has_value()) {
            auto typed_ptr = std::static_pointer_cast<PlaceHolderType<std::vector<T>>>(placeholder_);
            auto &optional = *typed_ptr;
            optional->push_back(default_value.value());
        }
    }

    OptionTable::Row to_string() const {
        std::string allowed_values_str;
        if (!allowed_values_.empty()) {
            std::stringstream ss;
            ss << " ";
            constexpr char kSeparator[] = " , ";
            for (auto &v : allowed_values_) {
                ss << v.string() << kSeparator;
            }

            // Erase last separator
            allowed_values_str = ss.str();
            allowed_values_str.erase(allowed_values_str.length() - strlen(kSeparator));
            allowed_values_str += " ";
        }

        return {{
            config_.name,
            Variant::enum_to_str(type_),
            default_value_->string(),
            config_.help,
            allowed_values_str,
        }};
    }

    /// \returns True if set successfully, false if not (value is not allowed)
    bool set(const std::string &s) {
        assert(!multivalent_);

        switch (type_) {
        case Variant::Type::kString : return set_helper<std::string>(s); break;
        case Variant::Type::kUint64 : return set_helper<uint64_t>(s); break;
        case Variant::Type::kInt64  : return set_helper<int64_t>(s); break;
        case Variant::Type::kDouble : return set_helper<double>(s); break;
        case Variant::Type::kFloat  : return set_helper<float>(s); break;
        case Variant::Type::kBool   : return set_helper<bool>(s); break;
        default                     : assert(false);
        }

        return false;
    }

    /// \returns True if set successfully, false if not (value is not allowed)
    bool set(const std::vector<std::string> &s) {
        assert(multivalent_);

        switch (type_) {
        case Variant::Type::kString : return set_helper<std::string>(s); break;
        case Variant::Type::kUint64 : return set_helper<uint64_t>(s); break;
        case Variant::Type::kInt64  : return set_helper<int64_t>(s); break;
        case Variant::Type::kDouble : return set_helper<double>(s); break;
        case Variant::Type::kFloat  : return set_helper<float>(s); break;
        case Variant::Type::kBool   : return set_helper<bool>(s); break;
        default                     : assert(false);
        }

        return false;
    }

    const std::string &name() const {
        return config_.name;
    }

    char letter() const {
        return config_.letter;
    }

    Variant::Type type() const {
        return type_;
    }

    bool required() const {
        return config_.required;
    }

    bool multivalent() const {
        return multivalent_;
    }

  private:
    const Config config_;
    const Variant::Type type_;
    const pre_std::optional<Variant> default_value_;
    const std::unordered_set<Variant, Variant::hash> allowed_values_;
    const bool multivalent_ = false;

    /// Handle to value to be populated
    std::shared_ptr<void> placeholder_;

    /// Determines the value of [default_value_]
    template <typename T>
    pre_std::optional<Variant> determine_default_value(const pre_std::optional<T> &default_value) {
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
                const auto &allowed_value = v.get<T>();
                if (value == allowed_value) {
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                return false;
            }
        }

        auto typed_ptr = std::static_pointer_cast<PlaceHolderType<T>>(placeholder_);
        *typed_ptr = value;

        return true;
    }

    /// \returns True if set successfully, false if not (value is not allowed)
    template <typename T>
    bool set_helper(const std::vector<std::string> &s) {
        auto typed_ptr = std::static_pointer_cast<PlaceHolderType<std::vector<T>>>(placeholder_);
        auto &optional = *typed_ptr;
        optional.emplace();

        for (const auto &each : s) {
            const auto value = detail::convert_helper<T>(each);

            // Check
            if (!allowed_values_.empty()) {
                bool matched = false;
                for (const auto &v : allowed_values_) {
                    const auto &allowed_value = v.get<T>();
                    if (value == allowed_value) {
                        matched = true;
                        break;
                    }
                }
                if (!matched) {
                    return false;
                }
            }

            optional->push_back(value);
        }

        return true;
    }
};

} // namespace argparse
