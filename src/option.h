#pragma once

#include "convert.h"
#include "option_helpers.h"
#include "std_variant.h"
#include "table.h"

#include <cassert>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>

namespace argparse {

class Option {
  public:
    using OptionTable = Table<4, Alignment::Center>;

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
    Option(std::shared_ptr<T> value, Config config, const T &default_value)
        : config_(std::move(config)),
          type_(detail::deduce_variant<T>()),
          default_value_(detail::make_variant(default_value)),
          has_default_(true),
          value_handle_(value) {
        assert(value_handle_);
    }

    OptionTable::Row to_string() const {
        // Stringify default value
        std::stringstream ss;
        if (has_default_) {
            ss << default_value_;
        }

        return {{
            config_.name,
            enum_to_str(type_),
            ss.str(),
            config_.help
        }};
    }

    void set(const std::string &s) {
        switch (type_) {
        case kString : set_helper<std::string>(s); break;
        case kUint64 : set_helper<uint64_t>(s); break;
        case kInt64  : set_helper<int64_t>(s); break;
        case kDouble : set_helper<double>(s); break;
        case kFloat  : set_helper<float>(s); break;
        case kBool   : set_helper<bool>(s); break;
        default      : assert(false);
        }
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
    const V default_value_;
    const bool has_default_;

    /// Current value of the option
    V value_;

    /// Handle to value to be populated
    std::shared_ptr<void> value_handle_;

    template <typename T>
    void set_helper(const std::string &s) {
        const auto value = detail::convert_helper<T>(s);
        value_.emplace<T>(value);
        auto typed_ptr = std::static_pointer_cast<T>(value_handle_);
        *typed_ptr = value;
    }
};

} // namespace argparse
