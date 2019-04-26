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
    constexpr int argc = 6;
    const char *argv[] = {
        "path",
        "--verbose",
        kVerboseLevel,
        "--mode",
        kMode,
        "--path",
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

TEST_CASE("[Parsing] help", "") {
    uint32_t num_times_help_called = 0;

    auto make_parser = [&num_times_help_called](const bool required) {
        Parser p("Sample Program", "Testing...");
        p.add<std::string>({
            .name = "mode",
            .required = required,
        }, "a");
        p.set_help_callback([&num_times_help_called] { num_times_help_called++; });
        p.set_exit_callback([]{});
        return p;
    };

    // No required args, so none are missing
    {
        constexpr int argc = 0;
        const char *argv[] = {
        };

        num_times_help_called = 0;
        auto p = make_parser(false);
        p.parse(argc, argv);
        REQUIRE(num_times_help_called == 0);
    }

    // One required arg, so one is missing
    {
        constexpr int argc = 0;
        const char *argv[] = {
        };

        num_times_help_called = 0;
        auto p = make_parser(true);
        p.parse(argc, argv);
        REQUIRE(num_times_help_called == 1);
    }

    // No required args, so none are missing, but -h is provided
    {
        constexpr int argc = 2;
        const char *argv[] = {
            "path",
            "-h",
        };

        num_times_help_called = 0;
        auto p = make_parser(false);
        p.parse(argc, argv);
        REQUIRE(num_times_help_called == 1);
    }

    // No required args, so none are missing, but --help is provided
    {
        constexpr int argc = 2;
        const char *argv[] = {
            "path",
            "--help",
        };

        num_times_help_called = 0;
        auto p = make_parser(false);
        p.parse(argc, argv);
        REQUIRE(num_times_help_called == 1);
    }
}

TEST_CASE("[Parsing] parser", "") {
    // One option no value
    {
        argparse::detail::Parser p;
        constexpr int argc = 2;
        const char *argv[argc] = {
            "path",
            "--d"
        };

        p.parse(argc, argv);
        const auto &args = p.args();
        REQUIRE(args.size() == 1);
        REQUIRE(args.find("d") != args.end());
        REQUIRE(args.at("d").empty());
    }

    // One option no value
    {
        argparse::detail::Parser p;
        constexpr int argc = 2;
        const char *argv[argc] = {
            "path",
            "-d"
        };

        p.parse(argc, argv);
        const auto &args = p.args();
        REQUIRE(args.size() == 1);
        REQUIRE(args.find("d") != args.end());
        REQUIRE(args.at("d").empty());
    }

    // One option one value
    {
        argparse::detail::Parser p;
        constexpr int argc = 3;
        const char *argv[argc] = {
            "path",
            "--d",
            "value",
        };

        p.parse(argc, argv);
        const auto &args = p.args();
        REQUIRE(args.size() == 1);
        REQUIRE(args.find("d") != args.end());
        REQUIRE(args.at("d").size() == 1);
        REQUIRE(args.at("d")[0] == "value");
    }

    // One option 5 values
    {
        argparse::detail::Parser p;
        constexpr int argc = 7;
        constexpr std::size_t kNumValues = 5;
        const char *argv[argc] = {
            "path",
            "--d",
            "value1",
            "value2",
            "value3",
            "value4",
            "value5",
        };

        p.parse(argc, argv);
        const auto &args = p.args();
        REQUIRE(args.size() == 1);
        REQUIRE(args.find("d") != args.end());
        REQUIRE(args.at("d").size() == kNumValues);
        const auto &vec = args.at("d");
        for (std::size_t ii = 0; ii < kNumValues; ii++) {
            REQUIRE(vec[ii] == argv[ii + 2]);
        }
    }

    // 3 options with {1, 2, 3} values respectively
    {
        argparse::detail::Parser p;
        constexpr int argc = 10;
        const char *argv[argc] = {
            "path",
            "--a",
            "value1",
            "--b",
            "value1",
            "value2",
            "--c",
            "value1",
            "value2",
            "value3",
        };

        p.parse(argc, argv);
        const auto &args = p.args();
        REQUIRE(args.size() == 3);
        REQUIRE(args.find("a") != args.end());
        REQUIRE(args.find("b") != args.end());
        REQUIRE(args.find("c") != args.end());
        REQUIRE(args.at("a").size() == 1);
        REQUIRE(args.at("b").size() == 2);
        REQUIRE(args.at("c").size() == 3);
        const auto &a = args.at("a");
        const auto &b = args.at("b");
        const auto &c = args.at("c");
        REQUIRE(a[0] == "value1");
        REQUIRE(b[0] == "value1");
        REQUIRE(b[1] == "value2");
        REQUIRE(c[0] == "value1");
        REQUIRE(c[1] == "value2");
        REQUIRE(c[2] == "value3");
    }
}
