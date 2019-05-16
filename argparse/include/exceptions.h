#pragma once

#include <exception>

namespace argparse {

class InvalidConfig : public std::exception {
  public:
    const char *what() const noexcept override {
        return "Configuration is invalid";
    }
};

} // namespace argparse
