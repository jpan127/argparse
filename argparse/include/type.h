#pragma once

/// Supported types
enum class Type {
    kNone,   /// Holds no value yet
    kString, /// std::string
    kDouble, /// double
    kFloat,  /// float
    kUint64, /// uint64_t
    kInt64,  /// int64_t
    kUint32, /// uint32_t
    kInt32,  /// int32_t
    KUint16, /// uint16_t
    KInt16,  /// int16_t
    kUint8,  /// uint8_t
    kInt8,   /// int8_t
    kBool,   /// bool
    kChar,   /// char
};

/// Convert [Type] to string
constexpr const char * enum_to_str(const Type type) {
    switch (type) {
    case Type::kString : return "string";   break;
    case Type::kDouble : return "double";   break;
    case Type::kFloat  : return "float";    break;
    case Type::kUint64 : return "uint64_t"; break;
    case Type::kInt64  : return "int64_t";  break;
    case Type::kUint32 : return "uint32_t"; break;
    case Type::kInt32  : return "int32_t";  break;
    case Type::KUint16 : return "uint16_t"; break;
    case Type::KInt16  : return "int16_t";  break;
    case Type::kUint8  : return "uint8_t";  break;
    case Type::kInt8   : return "int8_t";   break;
    case Type::kBool   : return "bool";     break;
    case Type::kChar   : return "char";     break;
    case Type::kNone:
    default:
        break;
    }

    return nullptr;
}
