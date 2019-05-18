#include "catch.hpp"

#include "argparse.h"
#include "utilities.h"
using namespace argparse;

/// Tests parsing of positional arguments
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

/// Tests parsing of positional arguments of subparsers
TEST_CASE("PositionalSubparser", "Parsing") {
    Parser p;
    replace_exit_cb(p);

    auto &subparsers = p.add_subparser("mode", {"write", "read", "append"});

    auto &write = subparsers["write"];
    auto &read = subparsers["read"];
    auto &append = subparsers["append"];

    const auto &output_path = write.add_leading_positional<std::string>({.name = "output_path"});
    const auto &read_input_path = read.add_leading_positional<std::string>({.name = "read_input_path"});
    const auto &append_input_path = append.add_leading_positional<std::string>({.name = "append_input_path"});
    const auto &append_data = append.add_leading_positional<std::string>({.name = "append_data"});

    const auto &permissions = p.add<std::string>({.name = "permissions"});
    const auto &compression = p.add<std::string>({.name = "compression"});

    SECTION("write") {
        constexpr int argc = 5;
        const char *argv[argc] = {
            "path",
            "write",
            "~/test",
            "--permissions",
            "+x",
        };

        p.parse(argc, argv);

        REQUIRE(output_path->has_value());
        REQUIRE(!read_input_path->has_value());
        REQUIRE(!append_input_path->has_value());
        REQUIRE(!append_data->has_value());

        REQUIRE(output_path->value() == "~/test");
    }

    SECTION("append") {
        constexpr int argc = 6;
        const char *argv[argc] = {
            "path",
            "append",
            "/tmp/my/data",
            "some_data_some_data_some_data_some_data_some_data_some_data",
            "--compression",
            "gzip",
        };

        p.parse(argc, argv);

        REQUIRE(!output_path->has_value());
        REQUIRE(!read_input_path->has_value());
        REQUIRE(append_input_path->has_value());
        REQUIRE(append_data->has_value());

        REQUIRE(append_input_path->value() == "/tmp/my/data");
        REQUIRE(append_data->value() == "some_data_some_data_some_data_some_data_some_data_some_data");
    }
}
