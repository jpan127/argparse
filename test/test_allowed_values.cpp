#include "catch.hpp"

#include "argparse.h"
#include "utilities.h"
using namespace argparse;

/// Tests that values must be one of the allowed values
TEST_CASE("AllowedValues", "Parsing") {
    Parser p;
    replace_exit_cb(p);
    const auto &mode = p.add(argparse::Config<std::string>{
        .default_value = {},
        .allowed_values = {"walk", "run", "jog", "skip"},
        .name = "mode",
        .help = "",
        .required = true,
    });

    constexpr int argc = 3;

    SECTION("Correct") {
        const char *argv[argc] = {
            "path",
            "--mode",
            "walk",
        };

        p.parse(argc, argv);

        REQUIRE(*mode == "walk");
    }

    SECTION("Incorrect") {
        const char *argv[argc] = {
            "path",
            "--mode",
            "sprint",
        };

        bool called = false;
        Parser::Callbacks cbs;
        cbs.not_allowed = [&called](auto, auto) { called = true; };
        p.set_callbacks(std::move(cbs));

        p.parse(argc, argv);

        REQUIRE(!mode->has_value());
        REQUIRE(called);
    }
}
