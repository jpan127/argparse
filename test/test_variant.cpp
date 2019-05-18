#include "catch.hpp"

#include "variant.h"
using namespace argparse;

TEST_CASE("variant", "Parsing") {
    Variant var;

    struct Visitor {
        std::string operator()(std::string) { return "std::string"; };
        std::string operator()(double) { return "double"; };
        std::string operator()(float) { return "float"; };
        std::string operator()(uint64_t) { return "uint64_t"; };
        std::string operator()(int64_t) { return "int64_t"; };
        std::string operator()(uint32_t) { return "uint32_t"; };
        std::string operator()(int32_t) { return "int32_t"; };
        std::string operator()(uint16_t) { return "uint16_t"; };
        std::string operator()(int16_t) { return "int16_t"; };
        std::string operator()(uint8_t) { return "uint8_t"; };
        std::string operator()(int8_t) { return "int8_t"; };
        std::string operator()(bool) { return "bool"; };
        std::string operator()(char) { return "char"; };
    };

    SECTION("std::string") {
        const auto value = std::string("asdffdsa");
        var = value;
        const auto str = var.visit(Visitor{});
        REQUIRE(str == "std::string");
        REQUIRE(var == value);
    }

    SECTION("double") {
        const auto value = double{5.12315141209};
        var = value;
        const auto str = var.visit(Visitor{});
        REQUIRE(str == "double");
        REQUIRE(var == value);
    }

    SECTION("float") {
        const auto value = float{5.12315141209};
        var = value;
        const auto str = var.visit(Visitor{});
        REQUIRE(str == "float");
        REQUIRE(var == value);
    }

    SECTION("uint64_t") {
        const auto value = uint64_t{0xFFFFFFFF12312};
        var = value;
        const auto str = var.visit(Visitor{});
        REQUIRE(str == "uint64_t");
        REQUIRE(var == value);
    }

    SECTION("int64_t") {
        const auto value = int64_t{-999999777777};
        var = value;
        const auto str = var.visit(Visitor{});
        REQUIRE(str == "int64_t");
        REQUIRE(var == value);
    }

    SECTION("uint32_t") {
        const auto value = uint32_t{123456789};
        var = value;
        const auto str = var.visit(Visitor{});
        REQUIRE(str == "uint32_t");
        REQUIRE(var == value);
    }

    SECTION("int32_t") {
        const auto value = int32_t{-987533};
        var = value;
        const auto str = var.visit(Visitor{});
        REQUIRE(str == "int32_t");
        REQUIRE(var == value);
    }

    SECTION("uint16_t") {
        const auto value = uint16_t{777};
        var = value;
        const auto str = var.visit(Visitor{});
        REQUIRE(str == "uint16_t");
        REQUIRE(var == value);
    }

    SECTION("int16_t") {
        const auto value = int16_t{-677};
        var = value;
        const auto str = var.visit(Visitor{});
        REQUIRE(str == "int16_t");
        REQUIRE(var == value);
    }

    SECTION("uint8_t") {
        const auto value = uint8_t{222};
        var = value;
        const auto str = var.visit(Visitor{});
        REQUIRE(str == "uint8_t");
        REQUIRE(var == value);
    }

    SECTION("int8_t") {
        const auto value = int8_t{-53};
        var = value;
        const auto str = var.visit(Visitor{});
        REQUIRE(str == "int8_t");
        REQUIRE(var == value);
    }

    SECTION("bool") {
        const auto value = bool{true};
        var = value;
        const auto str = var.visit(Visitor{});
        REQUIRE(str == "bool");
        REQUIRE(var == value);
    }

    SECTION("char") {
        const auto value = char{'z'};
        var = value;
        const auto str = var.visit(Visitor{});
        REQUIRE(str == "char");
        REQUIRE(var == value);
    }
}
