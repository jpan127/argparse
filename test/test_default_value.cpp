#include "catch.hpp"

#include "argparse.h"
#include "utilities.h"
using namespace argparse;

TEST_CASE("DefaultValue", "Parsing") {
    Parser p;
    replace_exit_cb(p);
    const auto &mode = p.add<std::string>({
        .default_value = "walk",
        .name = "mode",
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
