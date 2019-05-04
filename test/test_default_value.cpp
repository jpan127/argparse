#include "catch.hpp"
#include "argparse.h"
#include "utilities.h"

using namespace argparse;

TEST_CASE("ChooseDefaultValue", "Parsing") {
    // Default value, no provided value
    {
        Parser p;
        replace_exit_cb(p);
        const auto &mode = p.add<std::string>({
            .name = "mode",
            .required = true,
        }, "walk");

        constexpr int argc = 2;
        const char *argv[argc] = {
            "path",
            "--mode",
        };

        p.parse(argc, argv);

        REQUIRE(mode != nullptr);
        REQUIRE(mode->value() == "walk");
    }

    // Default value, yes provided value
    {
        Parser p;
        replace_exit_cb(p);
        const auto &mode = p.add<std::string>({
            .name = "mode",
            .required = true,
        }, "walk");

        constexpr int argc = 3;
        const char *argv[argc] = {
            "path",
            "--mode",
            "sprint",
        };

        p.parse(argc, argv);

        REQUIRE(mode != nullptr);
        REQUIRE(mode->value() == "sprint");
    }
}
