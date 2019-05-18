#include "variant.h"

#include <cassert>
#include <memory>
#include <sstream>

namespace argparse {

Variant::Variant() { // NOLINT
}

Variant::~Variant() {
    destroy();
}

Variant::Variant(const Variant &other) { // NOLINT
    copy(other);
}

Variant::Variant(Variant &&other) noexcept { // NOLINT
    copy(other);
}

Variant &Variant::operator=(const Variant &other) {
    copy(other);
    return *this;
}

Variant &Variant::operator=(Variant &&other) noexcept {
    copy(other);
    return *this;
}

std::string Variant::string() const {
    std::stringstream ss;
    switch (type_) {
    case Type::kString : ss << string_;   break;
    case Type::kDouble : ss << double_;   break;
    case Type::kFloat  : ss << float_;    break;
    case Type::kUint64 : ss << uint64_t_; break;
    case Type::kInt64  : ss << int64_t_;  break;
    case Type::kUint32 : ss << uint32_t_; break;
    case Type::kInt32  : ss << int32_t_;  break;
    case Type::KUint16 : ss << uint16_t_; break;
    case Type::KInt16  : ss << int16_t_;  break;
    case Type::kUint8  : ss << uint8_t_;  break;
    case Type::kInt8   : ss << int8_t_;   break;
    case Type::kBool   : ss << bool_;     break;
    case Type::kChar   : ss << char_;     break;
    case Type::kNone   :
    default            :                  break;
    }

    return ss.str();
}

bool Variant::operator==(const Variant &other) const {
    if (type_ != other.type_) {
        return false;
    }

    switch (type_) {
    case Type::kString : return (string_ == other.string_);
    case Type::kDouble : return (double_ == other.double_);
    case Type::kFloat  : return (float_ == other.float_);
    case Type::kUint64 : return (uint64_t_ == other.uint64_t_);
    case Type::kInt64  : return (int64_t_ == other.int64_t_);
    case Type::kUint32 : return (uint32_t_ == other.uint32_t_);
    case Type::kInt32  : return (int32_t_ == other.int32_t_);
    case Type::KUint16 : return (uint16_t_ == other.uint16_t_);
    case Type::KInt16  : return (int16_t_ == other.int16_t_);
    case Type::kUint8  : return (uint8_t_ == other.uint8_t_);
    case Type::kInt8   : return (int8_t_ == other.int8_t_);
    case Type::kBool   : return (bool_ == other.bool_);
    case Type::kChar   : return (char_ == other.char_);
    case Type::kNone   :
    default            : assert(false);
    }

    return false;
}

bool Variant::operator==(const std::string &value) const { return (type_ == Type::kString && string_ == value);   }
bool Variant::operator==(const double &value) const      { return (type_ == Type::kDouble && double_ == value);   }
bool Variant::operator==(const float &value) const       { return (type_ == Type::kFloat  && float_ == value);    }
bool Variant::operator==(const uint64_t &value) const    { return (type_ == Type::kUint64 && uint64_t_ == value); }
bool Variant::operator==(const int64_t &value) const     { return (type_ == Type::kInt64  && int64_t_ == value);  }
bool Variant::operator==(const uint32_t &value) const    { return (type_ == Type::kUint32 && uint32_t_ == value); }
bool Variant::operator==(const int32_t &value) const     { return (type_ == Type::kInt32  && int32_t_ == value);  }
bool Variant::operator==(const uint16_t &value) const    { return (type_ == Type::KUint16 && uint16_t_ == value); }
bool Variant::operator==(const int16_t &value) const     { return (type_ == Type::KInt16  && int16_t_ == value);  }
bool Variant::operator==(const uint8_t &value) const     { return (type_ == Type::kUint8  && uint8_t_ == value);  }
bool Variant::operator==(const int8_t &value) const      { return (type_ == Type::kInt8   && int8_t_ == value);   }
bool Variant::operator==(const bool &value) const        { return (type_ == Type::kBool   && bool_ == value);     }
bool Variant::operator==(const char &value) const        { return (type_ == Type::kChar   && char_ == value);     }

void Variant::copy(const Variant &other) {
    switch (other.type_) {
    case Type::kString : set(other.string_);   break;
    case Type::kDouble : set(other.double_);   break;
    case Type::kFloat  : set(other.float_);    break;
    case Type::kUint64 : set(other.uint64_t_); break;
    case Type::kInt64  : set(other.int64_t_);  break;
    case Type::kUint32 : set(other.uint32_t_); break;
    case Type::kInt32  : set(other.int32_t_);  break;
    case Type::KUint16 : set(other.uint16_t_); break;
    case Type::KInt16  : set(other.int16_t_);  break;
    case Type::kUint8  : set(other.uint8_t_);  break;
    case Type::kInt8   : set(other.int8_t_);   break;
    case Type::kBool   : set(other.bool_);     break;
    case Type::kChar   : set(other.char_);     break;
    case Type::kNone   :
    default            : assert(false);
    }
}

void Variant::destroy() {
    if (type_ == Type::kString) {
        string_.~basic_string<char>();
    }
}

void Variant::set(const char *value) { destroy(); type_ = Type::kString; ::new (std::addressof(string_))std::string(value);            }
void Variant::set(std::string value) { destroy(); type_ = Type::kString; ::new (std::addressof(string_))std::string(std::move(value)); }
void Variant::set(double value)      { destroy(); type_ = Type::kDouble; ::new (std::addressof(double_))double(value);                 }
void Variant::set(float value)       { destroy(); type_ = Type::kFloat;  ::new (std::addressof(float_))float(value);                   }
void Variant::set(uint64_t value)    { destroy(); type_ = Type::kUint64; ::new (std::addressof(uint64_t_))uint64_t(value);             }
void Variant::set(int64_t value)     { destroy(); type_ = Type::kInt64;  ::new (std::addressof(int64_t_))int64_t(value);               }
void Variant::set(uint32_t value)    { destroy(); type_ = Type::kUint32; ::new (std::addressof(uint32_t_))uint32_t(value);             }
void Variant::set(int32_t value)     { destroy(); type_ = Type::kInt32;  ::new (std::addressof(int32_t_))int32_t(value);               }
void Variant::set(uint16_t value)    { destroy(); type_ = Type::KUint16; ::new (std::addressof(uint16_t_))uint16_t(value);             }
void Variant::set(int16_t value)     { destroy(); type_ = Type::KInt16;  ::new (std::addressof(int16_t_))int16_t(value);               }
void Variant::set(uint8_t value)     { destroy(); type_ = Type::kUint8;  ::new (std::addressof(uint8_t_))uint8_t(value);               }
void Variant::set(int8_t value)      { destroy(); type_ = Type::kInt8;   ::new (std::addressof(int8_t_))int8_t(value);                 }
void Variant::set(bool value)        { destroy(); type_ = Type::kBool;   ::new (std::addressof(bool_))bool(value);                     }
void Variant::set(char value)        { destroy(); type_ = Type::kChar;   ::new (std::addressof(char_))char(value);                     }

std::size_t Variant::hash::operator()(const Variant &v) const {
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
    case Type::kNone   :
    default            : assert(false);
    }
}

} // namespace argparse
