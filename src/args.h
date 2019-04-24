#pragma once

#include "option.h"
#include "option_helpers.h"
#include "std_optional.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace argparse {

class Args {
  public:
    void add(const std::string &name, const std::shared_ptr<Option> &option) {
        args_[name] = option;
    }

    template <typename T>
    pre_std::optional<T> get(const std::string &name) const {
        static_assert(detail::acceptable<T>(), "Must be a valid type");

        const auto iterator = args_.find(name);
        if (iterator != args_.end()) {
            const auto option = iterator->second;
            return option->convert<T>();
        }

        return {};
    }

    bool exists(const std::string &name) const {
        return args_.find(name) != args_.end();
    }

  private:
    std::unordered_map<std::string, std::shared_ptr<Option>> args_;
};

} // namespace argparse
