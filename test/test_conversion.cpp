#include "catch.hpp"

#include "convert.h"

TEST_CASE("UnsignedIntegers", "Conversion") {
    REQUIRE(5 == argparse::detail::convert<uint32_t>(uint64_t{5}));
    REQUIRE(5 == argparse::detail::convert<uint64_t>("5"));
}
