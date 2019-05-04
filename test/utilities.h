#pragma once

#include "argparse.h"

#include <iostream>

/// Catch2 exits on std::exit and considers exceptions as failures
/// However, sometimes we need to test exiting, so this prevents the failure
inline void replace_exit_cb(argparse::Parser &p) {
    p.set_callbacks({
        .exit = [] { std::cout << "Program exited\n"; }
    });
}
