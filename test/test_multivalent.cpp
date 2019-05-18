#include "catch.hpp"

#include "argparse.h"
#include "utilities.h"
using namespace argparse;

/// Tests parsing of multiple values per option
TEST_CASE("MultivalentOption", "Parsing") {
    auto test = [](const auto &mode, auto &p) {
        constexpr int argc = 6;
        const char *argv[argc] = {
            "path",
            "--mode",
            "walk",
            "jog",
            "skip",
            "fly",
        };

        p.parse(argc, argv);

        REQUIRE(mode->has_value());
        const auto &vector = mode->value();
        REQUIRE(vector.size() == argc - 2);
        REQUIRE(vector[0] == "walk");
        REQUIRE(vector[1] == "jog");
        REQUIRE(vector[2] == "skip");
        REQUIRE(vector[3] == "fly");
    };

    Parser p;
    replace_exit_cb(p);

    SECTION("No default value") {
        const auto &mode = p.add_multivalent<std::string>({
            .name = "mode",
            .required = true,
        });
        test(mode, p);
    }

    SECTION("Yes default value") {
        Parser p;
        replace_exit_cb(p);
        const auto &mode = p.add_multivalent<std::string>({
            .name = "mode",
            .required = true,
            .default_value = "bounce",
        });
        test(mode, p);
    }

    SECTION("Allowed values") {
        Parser p;
        replace_exit_cb(p);
        const auto &mode = p.add_multivalent<std::string>({
            .name = "mode",
            .required = true,
            .allowed_values = {"walk", "jog", "skip", "fly"},
        });
        test(mode, p);
    }
}
