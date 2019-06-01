#include "argparse.h"

#include <iostream>

using namespace argparse;

int main(int argc, char const *argv[]) {
    try {
        // Create parser
        Parser p("Sample Program", "Testing...");

        // Add positional arguments, they are added in calling order
        const auto mode = p.add_leading_positional(argparse::Config<std::string>{
            .default_value = {},
            .allowed_values = {"a", "b", "c"},
            .name = "mode",
            .help = "Operation mode of this program",
        });
        const auto path = p.add_leading_positional(argparse::Config<bool>{
            .default_value = false,
            .allowed_values = {},
            .name = "path",
            .help = "",
            .required = true,
        });

        // Add non-positional arguments
        const auto verbose = p.add(argparse::Config<uint64_t>{
            .default_value = 5,
            .allowed_values = {},
            .name = "verbose",
        });
        const auto id = p.add(argparse::Config<int64_t>{
            .default_value = {},
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