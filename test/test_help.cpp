#include "catch.hpp"

#include "argparse.h"
#include "utilities.h"
using namespace argparse;

TEST_CASE("HelpMessage", "Parsing") {
    uint32_t num_times_help_called = 0;

    Parser p("Sample Program", "Testing...");
    replace_exit_cb(p);
    p.set_callbacks({
        .help = [&num_times_help_called] { num_times_help_called++; },
    });

    SECTION("No required args, so none are missing") {
        constexpr int argc = 0;
        const char *argv[] = {
        };

        p.add<std::string>({
            .name = "mode",
            .required = false,
        }, "a");
        p.parse(argc, argv);
        REQUIRE(num_times_help_called == 0);
    }

    SECTION("One required arg, so one is missing") {
        constexpr int argc = 0;
        const char *argv[] = {
        };

        p.add<std::string>({
            .name = "mode",
            .required = true,
        }, "a");
        p.parse(argc, argv);
        REQUIRE(num_times_help_called == 1);
    }

    SECTION("No required args, so none are missing, but -h is provided") {
        constexpr int argc = 2;
        const char *argv[] = {
            "path",
            "-h",
        };

        p.add<std::string>({
            .name = "mode",
            .required = false,
        }, "a");
        p.parse(argc, argv);
        REQUIRE(num_times_help_called == 1);
    }

    SECTION("No required args, so none are missing, but --help is provided") {
        constexpr int argc = 2;
        const char *argv[] = {
            "path",
            "--help",
        };

        p.add<std::string>({
            .name = "mode",
            .required = false,
        }, "a");
        p.parse(argc, argv);
        REQUIRE(num_times_help_called == 1);
    }
}
