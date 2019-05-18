#pragma once

#include <exception>

namespace argparse {

/// Exception for when an input configuration is invalid
class InvalidConfig : public std::exception {
  public:
    const char *what() const noexcept override {
        return "Configuration is invalid";
    }
};

} // namespace argparse
