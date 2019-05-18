#pragma once

#include "type.h"

#include <cassert>
#include <string>

namespace argparse {

template <typename T>
constexpr bool supported() {
    if (std::is_same<T, std::string>::value) {
        return true;
    }

    return std::is_fundamental<T>::value;
}

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

    /// Any assignment operator, but bounded by valid union types
    template <typename T>
    Variant &operator=(T value) {
        set(value);
        return *this;
    }

    /// Converts the current value into a string
    std::string string() const;

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
        case Type::kNone:
        default:
            assert(false);
        }
    }

    struct hash {
        std::size_t operator()(const Variant &v) const {
            const auto type_hash = std::hash<std::size_t>{}(static_cast<std::size_t>(v.type_));
            switch (v.type_) {
            case Type::kString : return type_hash ^ std::hash<std::string>{}(v.string_);
            case Type::kDouble : return type_hash ^ std::hash<double>{}(v.double_);
            case Type::kFloat  : return type_hash ^ std::hash<float>{}(v.float_);
            case Type::kUint64 : return type_hash ^ std::hash<uint64_t>{}(v.uint64_t_);
            case Type::kInt64  : return type_hash ^ std::hash<int64_t>{}(v.int64_t_);
            case Type::kUint32 : return type_hash ^ std::hash<uint32_t>{}(v.uint32_t_);
            case Type::kInt32  : return type_hash ^ std::hash<int32_t>{}(v.int32_t_);
            case Type::KUint16 : return type_hash ^ std::hash<uint16_t>{}(v.uint16_t_);
            case Type::KInt16  : return type_hash ^ std::hash<int16_t>{}(v.int16_t_);
            case Type::kUint8  : return type_hash ^ std::hash<uint8_t>{}(v.uint8_t_);
            case Type::kInt8   : return type_hash ^ std::hash<int8_t>{}(v.int8_t_);
            case Type::kBool   : return type_hash ^ std::hash<bool>{}(v.bool_);
            case Type::kChar   : return type_hash ^ std::hash<char>{}(v.char_);
            case Type::kNone:
            default:
                assert(false);
            }
        }
    };

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

  private:
    /// Which type is currently constructed
    Type type_ = Type::kNone;

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

    void copy(const Variant &other);

    /// Only std::string needs a destructor, so destruct it when it is going out of scope
    void destroy();

    /// Calls any destructors, sets the tag, then placement new's the new value
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
};

} // namespace argparse
