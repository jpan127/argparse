#include "catch.hpp"

#include "argparse.h"
#include "utilities.h"
using namespace argparse;

/// Tests that values must be one of the allowed values
TEST_CASE("AllowedValues", "Parsing") {
    Parser p;
    replace_exit_cb(p);
    const auto &mode = p.add<std::string>({
        .allowed_values = {"walk", "run", "jog", "skip"},
        .name = "mode",
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
        p.set_callbacks({
            .not_allowed = [&called](auto, auto) { called = true; }
        });

        p.parse(argc, argv);

        REQUIRE(!mode->has_value());
        REQUIRE(called);
    }
}
