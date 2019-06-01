#include "catch.hpp"

#include "argparse.h"
#include "utilities.h"
using namespace argparse;

/// Tests that values are set to the default value if provided
TEST_CASE("DefaultValue", "Parsing") {
    Parser p;
    replace_exit_cb(p);
    const auto &mode = p.add(argparse::Config<std::string>{
        .default_value = "walk",
        .allowed_values = {},
        .name = "mode",
        .help = "",
        .required = true,
    });

    SECTION("NoInput") {
        constexpr int argc = 2;
        const char *argv[argc] = {
            "path",
            "--mode",
        };

        p.parse(argc, argv);

        REQUIRE(mode->value() == "walk");
    }

    SECTION("YesInput") {
        constexpr int argc = 3;
        const char *argv[argc] = {
            "path",
            "--mode",
            "sprint",
        };

        p.parse(argc, argv);

        REQUIRE(mode->value() == "sprint");
    }
}
