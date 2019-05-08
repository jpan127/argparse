#include "catch.hpp"

#include "argparse.h"
#include "utilities.h"
using namespace argparse;

TEST_CASE("Subparser", "Subparsing") {
    Parser p;
    auto &subparsers = p.add_subparser("mode", {"play", "stop", "pause"});
    REQUIRE(subparsers.size() == 3);

    auto &play = subparsers["play"];
    auto &stop = subparsers["stop"];
    auto &pause = subparsers["pause"];

    const auto &song_name = play.add<std::string>(Option::Config{.name = "song_name"});
    const auto &delay = stop.add<uint64_t>(Option::Config{.name = "delay"});
    const auto &seconds = pause.add<uint64_t>(Option::Config{.name = "seconds"});

    SECTION("play") {
        constexpr int argc = 8;
        const char *argv[argc] = {
            "path",
            "play",
            "--song_name",
            "Old Town Road",
            "--delay",
            "50",
            "--seconds",
            "60",
        };

        const auto &remaining_args = p.parse(argc, argv);
        REQUIRE(p.subparser() == "play");

        REQUIRE(song_name->has_value());
        REQUIRE(!delay->has_value());
        REQUIRE(!seconds->has_value());

        REQUIRE(song_name->value() == "Old Town Road");

        REQUIRE(remaining_args.size() == 2);
        REQUIRE(remaining_args.exists("delay"));
        REQUIRE(remaining_args.exists("seconds"));
    }

    SECTION("stop") {
        constexpr int argc = 8;
        const char *argv[argc] = {
            "path",
            "stop",
            "--song_name",
            "Old Town Road",
            "--delay",
            "50",
            "--seconds",
            "60",
        };

        const auto &remaining_args = p.parse(argc, argv);
        REQUIRE(p.subparser() == "stop");

        REQUIRE(!song_name->has_value());
        REQUIRE(delay->has_value());
        REQUIRE(!seconds->has_value());

        REQUIRE(delay->value() == 50);

        REQUIRE(remaining_args.size() == 2);
        REQUIRE(remaining_args.exists("song_name"));
        REQUIRE(remaining_args.exists("seconds"));
    }

    SECTION("pause") {
        constexpr int argc = 8;
        const char *argv[argc] = {
            "path",
            "pause",
            "--song_name",
            "Old Town Road",
            "--delay",
            "50",
            "--seconds",
            "60",
        };

        const auto &remaining_args = p.parse(argc, argv);
        REQUIRE(p.subparser() == "pause");

        REQUIRE(!song_name->has_value());
        REQUIRE(!delay->has_value());
        REQUIRE(seconds->has_value());

        REQUIRE(seconds->value() == 60);

        REQUIRE(remaining_args.size() == 2);
        REQUIRE(remaining_args.exists("song_name"));
        REQUIRE(remaining_args.exists("delay"));
    }
}
