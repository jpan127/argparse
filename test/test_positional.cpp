#include "catch.hpp"

#include "argparse.h"
#include "utilities.h"
using namespace argparse;

TEST_CASE("PositionalArguments", "Parsing") {
    Parser p;
    replace_exit_cb(p);

    const auto &mode = p.add_leading_positional<std::string>({.name = "mode"});
    const auto &dir  = p.add_leading_positional<std::string>({.name = "dir"});
    const auto &file = p.add_leading_positional<std::string>({.name = "file"});

    const auto &type = p.add<std::string>({.name = "type"});
    const auto &permissions = p.add<std::string>({.name = "permissions"});
    const auto &compression = p.add<std::string>({.name = "compression"});

    SECTION("Only positional") {
        constexpr int argc = 4;
        const char *argv[argc] = {
            "path",
            "write",
            "~",
            "config.yaml",
        };

        p.parse(argc, argv);

        REQUIRE(mode->has_value());
        REQUIRE(dir->has_value());
        REQUIRE(file->has_value());

        REQUIRE("write" == mode->value());
        REQUIRE("~" == dir->value());
        REQUIRE("config.yaml" == file->value());
    }

    SECTION("Positional and non positional") {
        constexpr int argc = 10;
        const char *argv[argc] = {
            "path",
            "write",
            "~",
            "config.yaml",
            "--type",
            "yaml",
            "--permissions",
            "+x",
            "--compression",
            "gzip",
        };

        p.parse(argc, argv);

        REQUIRE(mode->has_value());
        REQUIRE(dir->has_value());
        REQUIRE(file->has_value());
        REQUIRE(type->has_value());
        REQUIRE(permissions->has_value());
        REQUIRE(compression->has_value());

        REQUIRE("write" == mode->value());
        REQUIRE("~" == dir->value());
        REQUIRE("config.yaml" == file->value());
        REQUIRE("yaml" == type->value());
        REQUIRE("+x" == permissions->value());
        REQUIRE("gzip" == compression->value());
    }
}
