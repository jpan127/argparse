#pragma once

#include "type.h"

#include <cassert>
#include <string>

namespace argparse {

/// Checks if a type is supported by this library
/// Either [std::string] or a fundamental type
template <typename T>
constexpr bool supported() {
    if (std::is_same<T, std::string>::value) {
        return true;
    }

    return std::is_fundamental<T>::value;
}

/// A specific and customized version of [std::variant]
/// The obect is implemented as a tagged union, and can only one of a set of types at a time
class Variant {
  public:
    /// Instantiate a constructor, as it is implicitly deleted
    Variant();

    /// Destructor must explicitly destruct std::string if that is constructed
    ~Variant();

    /// Any constructor type, but bounded by valid union types
    template <typename T>
    explicit Variant (T value) { // NOLINT
        set(value);
    }

    /// All of these copy the other variant
    Variant(const Variant &other);
    Variant(Variant &&other) noexcept;
    Variant &operator=(const Variant &other);
    Variant &operator=(Variant &&other) noexcept;

    /// Converts the current value into a string
    std::string string() const;

    /// Any assignment operator, but bounded by valid union types
    template <typename T>
    Variant &operator=(T value) {
        set(value);
        return *this;
    }

    /// Visitor method
    template <typename Visitor>
    decltype(auto) visit(Visitor &&visitor) {
        switch (type_) {
        case Type::kString : return visitor(string_);
        case Type::kDouble : return visitor(double_);
        case Type::kFloat  : return visitor(float_);
        case Type::kUint64 : return visitor(uint64_t_);
        case Type::kInt64  : return visitor(int64_t_);
        case Type::kUint32 : return visitor(uint32_t_);
        case Type::kInt32  : return visitor(int32_t_);
        case Type::KUint16 : return visitor(uint16_t_);
        case Type::KInt16  : return visitor(int16_t_);
        case Type::kUint8  : return visitor(uint8_t_);
        case Type::kInt8   : return visitor(int8_t_);
        case Type::kBool   : return visitor(bool_);
        case Type::kChar   : return visitor(char_);
        case Type::kNone   :
        default            : assert(false);
        }
    }

    /// @{ Equality operator for self and any of the supported types
    bool operator==(const Variant &other) const;
    bool operator==(const std::string &value) const;
    bool operator==(const double &value) const;
    bool operator==(const float &value) const;
    bool operator==(const uint64_t &value) const;
    bool operator==(const int64_t &value) const;
    bool operator==(const uint32_t &value) const;
    bool operator==(const int32_t &value) const;
    bool operator==(const uint16_t &value) const;
    bool operator==(const int16_t &value) const;
    bool operator==(const uint8_t &value) const;
    bool operator==(const int8_t &value) const;
    bool operator==(const bool &value) const;
    bool operator==(const char &value) const;
    /// @}

    /// Functor for hashing this object
    struct hash {
        std::size_t operator()(const Variant &v) const;
    };

  private:
    /// Which type is currently constructed
    Type type_ = Type::kNone;

    /// These are the only supported types and members
    /// Tagged union
    union {
        std::string string_;
        double double_;
        float float_;
        uint64_t uint64_t_;
        int64_t int64_t_;
        uint32_t uint32_t_;
        int32_t int32_t_;
        uint16_t uint16_t_;
        int16_t int16_t_;
        uint8_t uint8_t_;
        int8_t int8_t_;
        bool bool_;
        char char_;
    };

    /// Copies the value of another instance
    void copy(const Variant &other);

    /// Only std::string needs a destructor, so destruct it when it is going out of scope
    void destroy();

    /// @{ Calls any destructors, sets the tag, then placement new's the new value
    void set(const char *value);
    void set(std::string value);
    void set(double value);
    void set(float value);
    void set(uint64_t value);
    void set(int64_t value);
    void set(uint32_t value);
    void set(int32_t value);
    void set(uint16_t value);
    void set(int16_t value);
    void set(uint8_t value);
    void set(int8_t value);
    void set(bool value);
    void set(char value);
    /// @}
};

} // namespace argparse
