#pragma once

#include "config.h"
#include "placeholder.h"
#include "table.h"
#include "variant.h"

#include <memory>
#include <string>
#include <unordered_set>

namespace argparse {

class Option {
  public:
    static constexpr std::size_t kTableSize = 6;
    using OptionTable = Table<kTableSize, Alignment::Center>;

    /// Single Constructor
    template <typename T>
    Option(const PlaceHolder<T> &placeholder, Config<T> &&config);

    /// Multivalent Constructor
    template <typename T>
    Option(const PlaceHolder<std::vector<T>> &placeholder, Config<T> &&config);

    OptionTable::Row to_string() const;

    /// \returns True if set successfully, false if not (value is not allowed)
    bool set(const std::string &s);

    /// \returns True if set successfully, false if not (value is not allowed)
    bool set(const std::vector<std::string> &s);

    const std::string &name() const noexcept { return name_; }
    Variant::Type type() const noexcept { return type_; }
    bool required() const noexcept { return required_; }
    bool multivalent() const noexcept { return multivalent_; }
    bool positional() const noexcept { return positional_; }

  private:
    const Variant::Type type_;
    const pstd::optional<Variant> default_value_;
    const std::unordered_set<Variant, Variant::hash> allowed_values_;
    const std::string name_;
    const std::string help_;
    const bool multivalent_;
    const bool positional_;
    const bool required_;

    /// Handle to value to be populated
    std::shared_ptr<void> placeholder_;

    /// Determines the value of [default_value_]
    template <typename T>
    pstd::optional<Variant> determine_default_value(const pstd::optional<T> &default_value);

    template <typename T>
    bool set_dispatch_helper(const T &s);

    /// \returns True if set successfully, false if not (value is not allowed)
    template <typename T>
    bool set_helper(const std::string &s);

    /// \returns True if set successfully, false if not (value is not allowed)
    template <typename T>
    bool set_helper(const std::vector<std::string> &s);
};

} // namespace argparse
