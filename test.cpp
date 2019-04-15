#include "argparse.h"

using namespace argparse;

int main(int argc, char const *argv[])
{
    Parser p("Sample Program", "Testing...");
    // Option<uint64_t> o;
    // o.name = "verbose";
    p.add<uint64_t>({.name = "verbose"}, 5);
    p.parse(argc, argv);
    p.help();

    // OptionsVariant var;
    // var.emplace<1>(32);
    // std::cout << var.index() << std::endl;
    return 0;
}