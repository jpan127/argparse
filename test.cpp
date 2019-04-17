#include "argparse.h"

#include <array>

using namespace argparse;

int main(int argc, char const *argv[])
{
    Parser p("Sample Program", "Testing...");
    p.add<uint64_t>({
        .name = "verbose"
    }, 5);
    p.add<std::string>({
        .name = "mode",
        .help = "Select one of the options [a,b,c]"
    }, "a");
    p.add<bool>({
        .name = "path",
        .help = "",
        .required = true
    }, false);

    const auto &args = p.parse(argc, argv);
    std::cout << args.get<uint64_t>("verbose") << std::endl;
    std::cout << args.get<std::string>("mode") << std::endl;
    std::cout << args.get<bool>("path") << std::endl;

    return 0;
}