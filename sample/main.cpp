#include "argparse.h"

using namespace argparse;

int main(int argc, char const *argv[])
{
    uint64_t kDefaultVerboseLevel = 5;
    // Create parser
    Parser p("Sample Program", "Testing...");
    {
        p.add<uint64_t>({
            .name = "verbose"
        }, kDefaultVerboseLevel);
        p.add<std::string>({
            .name = "mode",
            .help = "Operation mode of this program"
        }, {"a", "b", "c"});
        p.add<bool>({
            .name = "path",
            .help = "",
            .required = true
        }, false);
        p.add<int64_t>({
            .name = "id",
            .help = "some identification number"
        }, {1234, 9999, 127127127});
    }

    p.parse(argc, argv);

    return 0;
}