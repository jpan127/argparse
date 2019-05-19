#pragma once

#include "config.h"
#include "placeholder.h"
#include "table.h"
#include "variant.h"

#include <memory>
#include <string>
#include <unordered_set>

namespace argparse {

/// Represents a single option and its configuration
class Option {
  public:
    /// The number of columns / fields needed to describe an option
    static constexpr std::size_t kTableSize = 7;
    using OptionTable = Table<kTableSize, Alignment::Center>;

    /// Single Constructor
    /// \param placeholder Placeholder pointer to a previously allocated object, the value will be populated later
    /// \param config Configuration for the option
    template <typename T>
    Option(const PlaceHolder<T> &placeholder, Config<T> &&config);

    /// Multivalent Constructor
    /// \param placeholder Placeholder pointer to a previously allocated object, the value will be populated later
    /// \param config Configuration for the option
    template <typename T>
    Option(const PlaceHolder<std::vector<T>> &placeholder, Config<T> &&config);

    /// Populates a row of string information about this option
    OptionTable::Row to_string() const;

    /// Sets the value of this option
    /// \returns True if set successfully, false if not (value is not allowed)
    bool set(const std::string &s);

    /// Sets the value of this option
    /// \returns True if set successfully, false if not (value is not allowed)
    bool set(const std::vector<std::string> &s);

    /// @{ Gets configuration details about this option
    const std::string &name() const noexcept { return name_; }
    char letter() const noexcept { return letter_; }
    Type type() const noexcept { return type_; }
    bool required() const noexcept { return required_; }
    bool multivalent() const noexcept { return multivalent_; }
    bool positional() const noexcept { return positional_; }
    /// @}

  private:
    /// Enumeration of the option type
    const Type type_;

    /// @{ Decomposed members of the configuration
    const pstd::optional<Variant> default_value_;
    const std::unordered_set<Variant, Variant::hash> allowed_values_;
    const std::string name_;
    const std::string help_;
    const char letter_;
    const bool multivalent_;
    const bool positional_;
    const bool required_;
    /// @}

    /// Handle to value to be populated
    std::shared_ptr<void> placeholder_;

    /// Determines the value of [default_value_]
    /// Converts [T] to [Variant]
    template <typename T>
    pstd::optional<Variant> determine_default_value(const pstd::optional<T> &default_value);

    /// Depending on the type, calls [set_helper] with the appropriate template type
    /// The type [T] of this function is not the type of the option, it is the type of the values
    ///   - Either [std::string] or [std::vector<std::string>]
    /// \param s Values to set
    /// \returns True if set successfully, false if not (value is not allowed)
    template <typename T>
    bool set_dispatch_helper(const T &s);

    /// Sets the value
    /// \returns True if set successfully, false if not (value is not allowed)
    template <typename T>
    bool set_helper(const std::string &s);

    /// Sets the values
    /// \returns True if set successfully, false if not (value is not allowed)
    template <typename T>
    bool set_helper(const std::vector<std::string> &s);
};

} // namespace argparse
