#include "catch.hpp"

#include "argparse.h"
#include "utilities.h"
using namespace argparse;

/// Tests parsing of subparser arguments
TEST_CASE("Subparser", "Subparsing") {
    Parser p;
    auto &subparsers = p.add_subparser("mode", {"play", "stop", "pause"});
    REQUIRE(subparsers.size() == 3);

    auto &play = subparsers["play"];
    auto &stop = subparsers["stop"];
    auto &pause = subparsers["pause"];

    const auto &song_name = play.add(argparse::Config<std::string>{.default_value = {}, .allowed_values = {}, .name = "song_name"});
    const auto &delay = stop.add(argparse::Config<uint64_t>{.default_value = {}, .allowed_values = {}, .name = "delay"});
    const auto &seconds = pause.add(argparse::Config<uint64_t>{.default_value = {}, .allowed_values = {}, .name = "seconds"});

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

        p.parse(argc, argv);
        REQUIRE(p.subparser() == "play");

        REQUIRE(song_name->has_value());
        REQUIRE(!delay->has_value());
        REQUIRE(!seconds->has_value());

        REQUIRE(song_name->value() == "Old Town Road");
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

        p.parse(argc, argv);
        REQUIRE(p.subparser() == "stop");

        REQUIRE(!song_name->has_value());
        REQUIRE(delay->has_value());
        REQUIRE(!seconds->has_value());

        REQUIRE(delay->value() == 50);
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

        p.parse(argc, argv);
        REQUIRE(p.subparser() == "pause");

        REQUIRE(!song_name->has_value());
        REQUIRE(!delay->has_value());
        REQUIRE(seconds->has_value());

        REQUIRE(seconds->value() == 60);
    }
}
