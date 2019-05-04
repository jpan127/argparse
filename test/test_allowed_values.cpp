#include "catch.hpp"

#include "argparse.h"
#include "utilities.h"
using namespace argparse;

TEST_CASE("AllowedValues", "Parsing") {
    Parser p;
    replace_exit_cb(p);
    const auto &mode = p.add<std::string>({
        .name = "mode",
        .required = true,
    }, {"walk", "run", "jog", "skip"});

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
            .invalid = [&called](auto, auto) { called = true; }
        });

        p.parse(argc, argv);

        REQUIRE(!mode->has_value());
        REQUIRE(called);
    }
}
