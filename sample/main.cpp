#include "argparse.h"

#include <iostream>

using namespace argparse;

int main(int argc, char const *argv[])
{
    constexpr uint64_t kDefaultVerboseLevel = 5;

    try {
        // Create parser
        Parser p("Sample Program", "Testing...");

        // Add positional arguments, they are added in calling order
        const auto mode = p.add_leading_positional<std::string>({
            .allowed_values = {"a", "b", "c"},
            .name = "mode",
            .help = "Operation mode of this program",
        });
        const auto path = p.add_leading_positional<bool>({
            .default_value = false,
            .name = "path",
            .help = "",
            .required = true,
        });

        // Add non-positional arguments
        const auto verbose = p.add<uint64_t>({
            .default_value = kDefaultVerboseLevel,
            .name = "verbose",
        });
        const auto id = p.add<int64_t>({
            .allowed_values = {1234, 9999, 127127127},
            .name = "id",
            .help = "some identification number",
        });

        // Parse arguments
        p.parse(argc, argv);

        // Print parsed arguments
        auto print = [](const auto &name, const auto &value) {
            std::cout << name << ": ";
            if (value->has_value()) {
                std::cout << value->value();
            }
            std::cout << std::endl;
        };

        print("mode", mode);
        print("path", path);
        print("verbose", verbose);
        print("id", id);
    } catch (const std::exception &e) {
    }

    return 0;
}