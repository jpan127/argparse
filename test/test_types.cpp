#include "catch.hpp"

#include "argparse.h"
#include "utilities.h"
using namespace argparse;

/// Tests parsing of all supported types
TEST_CASE("AllTypes", "Parsing") {
    Parser p;
    replace_exit_cb(p);

    const auto opt_std_string = p.add<std::string>({.name = "opt_std_string"});
    const auto opt_double = p.add<double>({.name = "opt_double"});
    const auto opt_float = p.add<float>({.name = "opt_float"});
    const auto opt_uint64_t = p.add<uint64_t>({.name = "opt_uint64_t"});
    const auto opt_int64_t = p.add<int64_t>({.name = "opt_int64_t"});
    const auto opt_uint32_t = p.add<uint32_t>({.name = "opt_uint32_t"});
    const auto opt_int32_t = p.add<int32_t>({.name = "opt_int32_t"});
    const auto opt_uint16_t = p.add<uint16_t>({.name = "opt_uint16_t"});
    const auto opt_int16_t = p.add<int16_t>({.name = "opt_int16_t"});
    const auto opt_uint8_t = p.add<uint8_t>({.name = "opt_uint8_t"});
    const auto opt_int8_t = p.add<int8_t>({.name = "opt_int8_t"});
    const auto opt_bool = p.add<bool>({.name = "opt_bool"});
    const auto opt_char = p.add<char>({.name = "opt_char"});

    constexpr int argc = 26;
    const char *argv[argc] = {
        "path",
        "--opt_std_string", "test_string",
        "--opt_double", "1.234",
        "--opt_float", "9.88888123812",
        "--opt_uint64_t", "120148012840123091",
        "--opt_int64_t", "-345534543",
        "--opt_uint32_t", "231233333",
        "--opt_int32_t", "-777777337",
        "--opt_uint16_t", "32000",
        "--opt_int16_t", "-32000",
        "--opt_uint8_t", "123",
        "--opt_int8_t", "-123",
        "--opt_bool",
        "--opt_char", "z",
    };

    p.parse(argc, argv);

    REQUIRE(opt_std_string->has_value());
    REQUIRE(opt_double->has_value());
    REQUIRE(opt_float->has_value());
    REQUIRE(opt_uint64_t->has_value());
    REQUIRE(opt_int64_t->has_value());
    REQUIRE(opt_uint32_t->has_value());
    REQUIRE(opt_int32_t->has_value());
    REQUIRE(opt_uint16_t->has_value());
    REQUIRE(opt_int16_t->has_value());
    REQUIRE(opt_uint8_t->has_value());
    REQUIRE(opt_int8_t->has_value());
    REQUIRE(opt_bool->has_value());
    REQUIRE(opt_char->has_value());

    REQUIRE(opt_std_string->value() == "test_string");
    REQUIRE(opt_double->value() == Approx(1.234));
    REQUIRE(opt_float->value() == Approx(9.88888123812));
    REQUIRE(opt_uint64_t->value() == 120148012840123091);
    REQUIRE(opt_int64_t->value() == -345534543);
    REQUIRE(opt_uint32_t->value() == 231233333);
    REQUIRE(opt_int32_t->value() == -777777337);
    REQUIRE(opt_uint16_t->value() == 32000);
    REQUIRE(opt_int16_t->value() == -32000);
    REQUIRE(opt_uint8_t->value() == 123);
    REQUIRE(opt_int8_t->value() == -123);
    REQUIRE(opt_bool->value() == true);
    REQUIRE(opt_char->value() == 'z');
}
