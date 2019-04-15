#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <cassert>

#include "options.h"

namespace argparse {


class Args {
  public:

  private:
    // std::vector<std::string> args_;
};

class Parser {
  public:
    Parser() = default;
    Parser(const std::string &name, const std::string &help) : name_(name), help_(help){}

    template <typename T>
    void add(const Option &options, const T default_value = {}) {
        options_.add<T>(options, default_value);
    }

    void help() const {
        std::cout << "Usage       : " << name_ << " [Options(s)]...\n"
                  << "Description : " << help_ << '\n'
                  << "Options     : " << std::endl;
        options_.display();
    }

    // const Args &
    void
    parse(const int argc, const char **argv) {
        // Parse args
        // for (std::size_t ii = 0; ii < argc; ii++) {
        //     std::cout << argv[ii] << std::endl;
        //     // Parse arg
        //     if (options_.find(argv[ii]) != options_.end()) {
        //         const auto o = options_[argv[ii]];
        //         const auto base_ptr = std::static_pointer_cast<OptionBase>(o);

        //         auto handle = [](auto ptr) {
        //             std::cout << "Found : " << ptr->name << std::endl;
        //             std::cout << "Type  : " << (int)ptr->type << std::endl;
        //         };
        //         switch (base_ptr->type) {
        //         case Type::String : handle(std::static_pointer_cast<Option<std::string>>(base_ptr)); break;
        //         case Type::Uint64 : handle(std::static_pointer_cast<Option<uint64_t>>(base_ptr)); break;
        //         case Type::Int64  : handle(std::static_pointer_cast<Option<int64_t>>(base_ptr)); break;
        //         case Type::Double : handle(std::static_pointer_cast<Option<double>>(base_ptr)); break;
        //         case Type::Float  : handle(std::static_pointer_cast<Option<float>>(base_ptr)); break;
        //         case Type::Bool   : handle(std::static_pointer_cast<Option<bool>>(base_ptr)); break;
        //         }
        //     }
        // }
    }

  private:
    const std::string name_ = "No Name";
    const std::string help_ = "No Help";
    Options options_;
    // std::map<std::string, std::shared_ptr<void>> options_{};
    // Args args_{};
};

} // namespace argparse
