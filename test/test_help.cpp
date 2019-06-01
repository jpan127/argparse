#include "catch.hpp"

#include "argparse.h"
#include "utilities.h"
using namespace argparse;

/// Tests that the help message is called at the appropriate times
TEST_CASE("HelpMessage", "Parsing") {
    uint32_t num_times_help_called = 0;

    Parser p("Sample Program", "Testing...");
    replace_exit_cb(p);

    Parser::Callbacks cbs;
    cbs.help = [&num_times_help_called] { num_times_help_called++; };
    p.set_callbacks(std::move(cbs));

    SECTION("No required args, so none are missing") {
        constexpr int argc = 1;
        const char *argv[] = {
            "path",
        };

        p.add(argparse::Config<std::string>{
            .default_value = "a",
            .allowed_values = {},
            .name = "mode",
            .help = "",
            .required = false,
        });
        p.parse(argc, argv);
        REQUIRE(num_times_help_called == 0);
    }

    SECTION("One required arg, so one is missing") {
        constexpr int argc = 1;
        const char *argv[] = {
            "path",
        };

        p.add(argparse::Config<std::string>{
            .default_value = "a",
            .allowed_values = {},
            .name = "mode",
            .help = "",
            .required = true,
        });
        p.parse(argc, argv);
        REQUIRE(num_times_help_called == 1);
    }

    SECTION("No required args, so none are missing, but -h is provided") {
        constexpr int argc = 2;
        const char *argv[] = {
            "path",
            "-h",
        };

        p.add(argparse::Config<std::string>{
            .default_value = "a",
            .allowed_values = {},
            .name = "mode",
            .help = "",
            .required = false,
        });
        p.parse(argc, argv);
        REQUIRE(num_times_help_called == 1);
    }

    SECTION("No required args, so none are missing, but --help is provided") {
        constexpr int argc = 2;
        const char *argv[] = {
            "path",
            "--help",
        };

        p.add(argparse::Config<std::string>{
            .default_value = "a",
            .allowed_values = {},
            .name = "mode",
            .help = "",
            .required = false,
        });
        p.parse(argc, argv);
        REQUIRE(num_times_help_called == 1);
    }
}
