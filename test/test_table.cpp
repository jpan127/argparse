#include "catch.hpp"

#include "table.h"
using namespace argparse;

/// Tests creating a small table with center alignment
TEST_CASE("CenterAlignment", "Table") {
    static constexpr char kExpected[] =
        "--------------------\n"
        "|first|second|third|\n"
        "--------------------\n"
        "|  a  |  b   |  c  |\n"
        "--------------------\n";

    Table<3> table(Table<3>::Row{{"first", "second", "third"}});
    table.add_row({{"a", "b", "c"}});
    REQUIRE(table.display() == std::string(kExpected));
}

/// Tests creating a small table with left alignment
TEST_CASE("LeftAlignment", "Table") {
    static constexpr char kExpected[] =
        "--------------------\n"
        "|first|second|third|\n"
        "--------------------\n"
        "|a    |b     |c    |\n"
        "--------------------\n";

    Table<3, Alignment::Left> table(Table<3, Alignment::Left>::Row{{"first", "second", "third"}});
    table.add_row({{"a", "b", "c"}});
    REQUIRE(table.display() == std::string(kExpected));
}

/// Tests creating a small table with right alignment
TEST_CASE("RightAlignment", "Table") {
    static constexpr char kExpected[] =
        "--------------------\n"
        "|first|second|third|\n"
        "--------------------\n"
        "|    a|     b|    c|\n"
        "--------------------\n";

    Table<3, Alignment::Right> table(Table<3, Alignment::Right>::Row{{"first", "second", "third"}});
    table.add_row({{"a", "b", "c"}});
    REQUIRE(table.display() == std::string(kExpected));
}

/// Tests creating a big table
TEST_CASE("Big", "Table") {
    static constexpr char kExpected[] =
        "-----------------------------------------------------\n"
        "|COLUMN_1 |COLUMN_2|  COLUMN_3   |COLUMN_4 |COLUMN_5|\n"
        "-----------------------------------------------------\n"
        "|  hello  |   my   |  beautiful  |  world  |   !    |\n"
        "| helloo  |  myy   | beautifull  | worldd  |   !!   |\n"
        "| hellooo |  myyy  | beautifulll | worlddd |  !!!   |\n"
        "|helloooo | myyyy  |beautifullll |worldddd |  !!!!  |\n"
        "|hellooooo| myyyyy |beautifulllll|worlddddd| !!!!!  |\n"
        "-----------------------------------------------------\n";

    Table<5> table(Table<5>::Row{{"COLUMN_1", "COLUMN_2", "COLUMN_3", "COLUMN_4", "COLUMN_5"}});
    table.add_row({{"hello", "my", "beautiful", "world", "!"}});
    table.add_row({{"helloo", "myy", "beautifull", "worldd", "!!"}});
    table.add_row({{"hellooo", "myyy", "beautifulll", "worlddd", "!!!"}});
    table.add_row({{"helloooo", "myyyy", "beautifullll", "worldddd", "!!!!"}});
    table.add_row({{"hellooooo", "myyyyy", "beautifulllll", "worlddddd", "!!!!!"}});
    REQUIRE(table.display() == std::string(kExpected));
}
