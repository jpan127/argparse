#include "catch.hpp"
#include "std_variant.h"
#include "variant.h"

#include <string>

TEST_CASE("", "") {
    pre_std::variant<
        std::string,
        uint64_t,
        int64_t,
        double,
        float,
        bool
    > v = false;

    struct Visitor {
        void operator()(std::string &value){};
        void operator()(uint64_t &value){};
        void operator()(int64_t &value){};
        void operator()(double &value){};
        void operator()(float &value){};
        void operator()(bool &value){};
    };

    pre_std::visit(Visitor{}, v);
}