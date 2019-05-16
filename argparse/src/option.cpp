#include "option.h"

#include "convert.h"

#include <cassert>
#include <sstream>

namespace argparse {

template <typename T>
Option::Option(const PlaceHolder<T> &placeholder, Config<T> &&config)
    : type_(detail::deduce_variant<T>()),
        default_value_(determine_default_value(config.default_value)),
        allowed_values_(detail::make_variants(config.allowed_values)),
        name_(std::move(config.name)),
        help_(std::move(config.help)),
        multivalent_(false),
        positional_(config.positional),
        required_(config.required),
        placeholder_(placeholder) {

    assert(placeholder_);

    // Set default value
    if (config.default_value.has_value()) {
        auto typed_ptr = std::static_pointer_cast<PlaceHolderType<T>>(placeholder_);
        *typed_ptr = config.default_value.value();
    }
}

/// Multivalent Constructor
template <typename T>
Option::Option(const PlaceHolder<std::vector<T>> &placeholder, Config<T> &&config)
    : type_(detail::deduce_variant<T>()),
        default_value_(determine_default_value(config.default_value)),
        allowed_values_(detail::make_variants(config.allowed_values)),
        name_(std::move(config.name)),
        help_(std::move(config.help)),
        multivalent_(true),
        positional_(config.positional),
        required_(config.required),
        placeholder_(placeholder) {

    assert(placeholder_);

    // Set default value
    if (config.default_value.has_value()) {
        auto typed_ptr = std::static_pointer_cast<PlaceHolderType<std::vector<T>>>(placeholder_);
        auto &optional = *typed_ptr;
        optional->push_back(config.default_value.value());
    }
}

Option::OptionTable::Row Option::to_string() const {
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
        required_ ? "x" : " ",
        name_,
        Variant::enum_to_str(type_),
        default_value_->string(),
        help_,
        allowed_values_str,
    }};
}

bool Option::set(const std::string &s) {
    assert(!multivalent_);
    return set_dispatch_helper(s);
}

bool Option::set(const std::vector<std::string> &s) {
    assert(multivalent_);
    return set_dispatch_helper(s);
}

template <typename T>
pstd::optional<Variant> Option::determine_default_value(const pstd::optional<T> &default_value) {
    if (default_value.has_value()) {
        return detail::make_variant(default_value.value());
    }

    return {};
}

template <typename T>
bool Option::set_dispatch_helper(const T &s) {
    switch (type_) {
    case Variant::Type::kString : return set_helper<std::string>(s);
    case Variant::Type::kDouble : return set_helper<double>(s);
    case Variant::Type::kFloat  : return set_helper<float>(s);
    case Variant::Type::kUint64 : return set_helper<uint64_t>(s);
    case Variant::Type::kInt64  : return set_helper<int64_t>(s);
    case Variant::Type::kUint32 : return set_helper<uint32_t>(s);
    case Variant::Type::kInt32  : return set_helper<int32_t>(s);
    case Variant::Type::KUint16 : return set_helper<uint16_t>(s);
    case Variant::Type::KInt16  : return set_helper<int16_t>(s);
    case Variant::Type::kUint8  : return set_helper<uint8_t>(s);
    case Variant::Type::kInt8   : return set_helper<int8_t>(s);
    case Variant::Type::kBool   : return set_helper<bool>(s);
    case Variant::Type::kChar   : return set_helper<char>(s);
    default                     : assert(false);
    }

    return false;
}

template <typename T>
bool Option::set_helper(const std::string &s) {
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

template <typename T>
bool Option::set_helper(const std::vector<std::string> &s) {
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

/// @{ Explicit Instantiation
template Option::Option(const PlaceHolder<std::string> &placeholder, Config<std::string> &&);
template Option::Option(const PlaceHolder<double> &placeholder, Config<double> &&);
template Option::Option(const PlaceHolder<float> &placeholder, Config<float> &&);
template Option::Option(const PlaceHolder<uint64_t> &placeholder, Config<uint64_t> &&);
template Option::Option(const PlaceHolder<int64_t> &placeholder, Config<int64_t> &&);
template Option::Option(const PlaceHolder<uint32_t> &placeholder, Config<uint32_t> &&);
template Option::Option(const PlaceHolder<int32_t> &placeholder, Config<int32_t> &&);
template Option::Option(const PlaceHolder<uint16_t> &placeholder, Config<uint16_t> &&);
template Option::Option(const PlaceHolder<int16_t> &placeholder, Config<int16_t> &&);
template Option::Option(const PlaceHolder<uint8_t> &placeholder, Config<uint8_t> &&);
template Option::Option(const PlaceHolder<int8_t> &placeholder, Config<int8_t> &&);
template Option::Option(const PlaceHolder<bool> &placeholder, Config<bool> &&);
template Option::Option(const PlaceHolder<char> &placeholder, Config<char> &&);
template Option::Option(const PlaceHolder<std::vector<std::string>> &placeholder, Config<std::string> &&);
template Option::Option(const PlaceHolder<std::vector<double>> &placeholder, Config<double> &&);
template Option::Option(const PlaceHolder<std::vector<float>> &placeholder, Config<float> &&);
template Option::Option(const PlaceHolder<std::vector<uint64_t>> &placeholder, Config<uint64_t> &&);
template Option::Option(const PlaceHolder<std::vector<int64_t>> &placeholder, Config<int64_t> &&);
template Option::Option(const PlaceHolder<std::vector<uint32_t>> &placeholder, Config<uint32_t> &&);
template Option::Option(const PlaceHolder<std::vector<int32_t>> &placeholder, Config<int32_t> &&);
template Option::Option(const PlaceHolder<std::vector<uint16_t>> &placeholder, Config<uint16_t> &&);
template Option::Option(const PlaceHolder<std::vector<int16_t>> &placeholder, Config<int16_t> &&);
template Option::Option(const PlaceHolder<std::vector<uint8_t>> &placeholder, Config<uint8_t> &&);
template Option::Option(const PlaceHolder<std::vector<int8_t>> &placeholder, Config<int8_t> &&);
template Option::Option(const PlaceHolder<std::vector<bool>> &placeholder, Config<bool> &&);
template Option::Option(const PlaceHolder<std::vector<char>> &placeholder, Config<char> &&);
/// @}

} // namespace argparse
