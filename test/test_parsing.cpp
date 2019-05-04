#include "catch.hpp"

#include "argparse.h"
#include "utilities.h"
using namespace argparse;

TEST_CASE("BasicArgument", "Parsing") {
    argparse::detail::Parser p;

    SECTION("One option no value") {
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

    SECTION("One option one value") {
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

    SECTION("One option 5 values") {
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

    SECTION("3 options with {1, 2, 3} values respectively") {
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

TEST_CASE("RemainingArguments", "Parsing") {
    Parser p("Sample Program", "Testing...");
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

    SECTION("3 options with {1, 2, 3} values respectively, none are expected") {
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

    SECTION("3 options with {1, 2, 3} values respectively, only one is expected") {
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

TEST_CASE("IndividualArguments", "Parsing") {
    constexpr int argc = 3;
    const char *argv[] = {
        "path",
        "--my_name",
        "JP",
    };

    Parser p;
    const auto my_name = p.add<std::string>("my_name", "my_help", 'a', true);
    p.parse(argc, argv);

    REQUIRE(my_name->has_value());
    REQUIRE(my_name->value() == "JP");
}

TEST_CASE("Letter", "Parsing") {
    Parser p;

    constexpr int argc = 3;
    const char *argv[] = {
        "path",
        "-d",
        "Wednesday",
    };

    SECTION("Only letter") {
        const auto day = p.add<std::string>({
            .letter = 'd',
            .required = true,
        });
        p.parse(argc, argv);

        REQUIRE(day->has_value());
        REQUIRE(day->value() == "Wednesday");
    }

    SECTION("Name and letter, but use letter") {
        const auto day = p.add<std::string>({
            .name = "day",
            .letter = 'd',
            .required = true,
        });
        p.parse(argc, argv);

        REQUIRE(day->has_value());
        REQUIRE(day->value() == "Wednesday");
    }

    SECTION("Name and letter, but use name") {
        const auto day = p.add<std::string>({
            .name = "day",
            .letter = 'd',
            .required = true,
        });
        p.parse(argc, argv);

        REQUIRE(day->has_value());
        REQUIRE(day->value() == "Wednesday");
    }
}
