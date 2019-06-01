#include "catch.hpp"

#include <algorithm>
#include <iostream>
#include "utils.h"

TEST_CASE("SplitValues", "Utils") {
    std::vector<std::string> values = {
        "l",
        "word",
        "word1,word2,word3",
        "a,b,c,d,e,f,g",
    };

    split_values(values);

    std::vector<std::string> correct = {
        "l", "word", "word1", "word2", "word3", "a", "b", "c", "d", "e", "f", "g",
    };

    std::sort(correct.begin(), correct.end());
    std::sort(values.begin(), values.end());

    REQUIRE(std::equal(correct.begin(), correct.end(), values.begin()));
}
