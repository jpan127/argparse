#include "argparse.h"

using namespace argparse;

int main(int argc, char const *argv[])
{
    uint64_t kDefaultVerboseLevel = 5;
    // Create parser
    Parser p("Sample Program", "Testing...");
    {
        p.add<uint64_t>(Option::Config{
            .name = "verbose"
        }, kDefaultVerboseLevel);
        p.add<std::string>(Option::Config{
            .name = "mode",
            .help = "Operation mode of this program"
        }, {"a", "b", "c"});
        p.add<bool>(Option::Config{
            .name = "path",
            .help = "",
            .required = true
        }, false);
        p.add<int64_t>(Option::Config{
            .name = "id",
            .help = "some identification number"
        }, {1234, 9999, 127127127});
    }

    p.parse(argc, argv);

    return 0;
}