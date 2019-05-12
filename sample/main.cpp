#include "argparse.h"

using namespace argparse;

int main(int argc, char const *argv[])
{
    uint64_t kDefaultVerboseLevel = 5;
    // Create parser
    Parser p("Sample Program", "Testing...");
    {
        p.add<uint64_t>({
            .default_value = kDefaultVerboseLevel,
            .name = "verbose",
        });
        p.add<std::string>({
            .allowed_values = {"a", "b", "c"},
            .name = "mode",
            .help = "Operation mode of this program",
        });
        p.add<bool>({
            .default_value = false,
            .name = "path",
            .help = "",
            .required = true,
        });
        p.add<int64_t>({
            .allowed_values = {1234, 9999, 127127127},
            .name = "id",
            .help = "some identification number",
        });
    }

    p.parse(argc, argv);

    return 0;
}