#include "catch.hpp"
#include "argparse.h"
#include "utilities.h"

using namespace argparse;

TEST_CASE("correct allowed values", "Parsing") {
    Parser p;
    replace_exit_cb(p);
    const auto &mode = p.add<std::string>({
        .name = "mode",
        .required = true,
    }, {"walk", "run", "jog", "skip"});

    constexpr int argc = 3;
    const char *argv[] = {
        "path",
        "--mode",
        "walk",
    };

    p.parse(argc, argv);

    REQUIRE(mode != nullptr);
    REQUIRE(*mode == "walk");
}

TEST_CASE("incorrect allowed values", "Parsing") {
    Parser p;
    replace_exit_cb(p);
    const auto &mode = p.add<std::string>({
        .name = "mode",
        .required = true,
    }, {"walk", "run", "jog", "skip"});

    constexpr int argc = 3;
    const char *argv[] = {
        "path",
        "--mode",
        "sprint",
    };

    bool called = false;
    p.set_invalid_callback([&called](auto, auto) { called = true; });

    p.parse(argc, argv);

    REQUIRE(mode != nullptr);
    REQUIRE(!mode->has_value());
    REQUIRE(called);
}
