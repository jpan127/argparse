#include "argparse.h"

using namespace argparse;

int main(int argc, char const *argv[])
{
    constexpr uint64_t kDefaultVerboseLevel = 5;
    // Create parser
    Parser p("Sample Program", "Testing...");
    {
        p.add<uint64_t>({
            .name = "verbose"
        }, kDefaultVerboseLevel);
        p.add<std::string>({
            .name = "mode",
            .help = "Select one of the options [a,b,c]"
        }, "a");
        p.add<bool>({
            .name = "path",
            .help = "",
            .required = true
        }, false);
    }

    p.parse(argc, argv);

    return 0;
}