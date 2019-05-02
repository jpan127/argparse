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
    const auto verbose = p.add<uint64_t>({
        .name = "verbose"
    }, kDefaultVerboseLevel);
    const auto mode = p.add<std::string>({
        .name = "mode",
        .help = "Select one of the options [a,b,c]"
    }, "a");
    const auto path = p.add<bool>({
        .name = "path",
        .help = "",
        .required = true
    }, false);

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

    p.parse(argc, argv);

    REQUIRE(verbose != nullptr);
    REQUIRE(mode != nullptr);
    REQUIRE(path != nullptr);

    REQUIRE(*verbose == 2);
    REQUIRE(*mode == std::string(kMode));
    REQUIRE(*path == true);
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

        const auto args = p.parse(argc, argv);
        REQUIRE(args.size() == 1);
        REQUIRE(args.exists('d'));
        REQUIRE(args.get('d').empty());
    }

    // One option no value
    {
        argparse::detail::Parser p;
        constexpr int argc = 2;
        const char *argv[argc] = {
            "path",
            "-d"
        };

        const auto args = p.parse(argc, argv);
        REQUIRE(args.size() == 1);
        REQUIRE(args.exists('d'));
        REQUIRE(args.get('d').empty());
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

        const auto args = p.parse(argc, argv);
        REQUIRE(args.size() == 1);
        REQUIRE(args.exists('d'));
        REQUIRE(args.get('d').size() == 1);
        REQUIRE(args.get('d')[0] == "value");
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

        const auto args = p.parse(argc, argv);
        REQUIRE(args.size() == 1);
        REQUIRE(args.exists('d'));
        REQUIRE(args.get('d').size() == kNumValues);
        const auto &vec = args.get('d');
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

        const auto args = p.parse(argc, argv);
        REQUIRE(args.size() == 3);
        REQUIRE(args.exists('a'));
        REQUIRE(args.exists('b'));
        REQUIRE(args.exists('c'));
        REQUIRE(args.get('a').size() == 1);
        REQUIRE(args.get('b').size() == 2);
        REQUIRE(args.get('c').size() == 3);
        const auto &a = args.get('a');
        const auto &b = args.get('b');
        const auto &c = args.get('c');
        REQUIRE(a[0] == "value1");
        REQUIRE(b[0] == "value1");
        REQUIRE(b[1] == "value2");
        REQUIRE(c[0] == "value1");
        REQUIRE(c[1] == "value2");
        REQUIRE(c[2] == "value3");
    }
}

TEST_CASE("[Parsing] remaining arguments", "") {
    // 3 options with {1, 2, 3} values respectively, none are expected
    {
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

        Parser p("Sample Program", "Testing...");
        const auto &remaining_args = p.parse(argc, argv);
        REQUIRE(remaining_args.size() == 3);
        REQUIRE(remaining_args.exists('a'));
        REQUIRE(remaining_args.exists('b'));
        REQUIRE(remaining_args.exists('c'));
        REQUIRE(remaining_args.get('a').size() == 1);
        REQUIRE(remaining_args.get('b').size() == 2);
        REQUIRE(remaining_args.get('c').size() == 3);
        const auto &a = remaining_args.get('a');
        const auto &b = remaining_args.get('b');
        const auto &c = remaining_args.get('c');
        REQUIRE(a[0] == "value1");
        REQUIRE(b[0] == "value1");
        REQUIRE(b[1] == "value2");
        REQUIRE(c[0] == "value1");
        REQUIRE(c[1] == "value2");
        REQUIRE(c[2] == "value3");
    }

    // 3 options with {1, 2, 3} values respectively, only one is expected
    {
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

        Parser p("Sample Program", "Testing...");
        p.add<std::string>({.letter = 'a'});

        const auto &remaining_args = p.parse(argc, argv);
        REQUIRE(remaining_args.size() == 2);
        REQUIRE(remaining_args.exists('b'));
        REQUIRE(remaining_args.exists('c'));
        REQUIRE(remaining_args.get('b').size() == 2);
        REQUIRE(remaining_args.get('c').size() == 3);
        const auto &b = remaining_args.get('b');
        const auto &c = remaining_args.get('c');
        REQUIRE(b[0] == "value1");
        REQUIRE(b[1] == "value2");
        REQUIRE(c[0] == "value1");
        REQUIRE(c[1] == "value2");
        REQUIRE(c[2] == "value3");
    }
}

TEST_CASE("[Parsing] add with individual arguments", "") {
    constexpr int argc = 3;
    const char *argv[] = {
        "path",
        "--my_name",
        "JP",
    };

    Parser p;
    const auto my_name = p.add<std::string>("my_name", "my_help", 'a', true);
    p.parse(argc, argv);

    REQUIRE(*my_name == "JP");
}
