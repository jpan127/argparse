#include "catch.hpp"
#include "argparse.h"

#include <cassert>

using namespace argparse;

TEST_CASE("[Conversion] unsigned integers", "") {
    REQUIRE(5 == argparse::detail::convert<uint32_t>(uint64_t{5}));
    REQUIRE(5 == argparse::detail::convert<uint64_t>("5"));
}

TEST_CASE("[Parsing] simple parsing", "") {
    constexpr uint64_t kDefaultVerboseLevel = 5;
    // Create parser
    Parser p("Sample Program", "Testing...");
    {
        p.add<uint64_t>({
            .name = "verbose"
        }, kDefaultVerboseLevel);
        p.add<std::string>({
            .name = "mode",
            .help = "Select one of the options [a,b,c]"
        }, "a");
        p.add<bool>({
            .name = "path",
            .help = "",
            .required = true
        }, false);
    }

    // Inputs
    constexpr const char *kVerboseLevel = "2";
    constexpr const char *kMode = "my_mode";
    constexpr const char *kUsePath = "true";
    constexpr int argc = 7;
    const char *argv[] = {
        "path",
        "--verbose",
        kVerboseLevel,
        "--mode",
        kMode,
        "--path",
        kUsePath,
    };

    const auto &args = p.parse(argc, argv);

    const auto verbose = args.get<uint64_t>("verbose");
    const auto mode = args.get<std::string>("mode");
    const auto path = args.get<bool>("path");

    REQUIRE(verbose.has_value());
    REQUIRE(mode.has_value());
    REQUIRE(path.has_value());

    REQUIRE(verbose.value() == 2);
    REQUIRE(mode.value() == std::string(kMode));
    REQUIRE(path.value() == true);
}
