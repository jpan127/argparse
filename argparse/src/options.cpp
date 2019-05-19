#include "options.h"

#include <memory>
#include <sstream>
#include <iostream>

namespace argparse {

template <typename T>
ConstPlaceHolder<T> Options::add(Config<T> &&config) {
    auto placeholder = std::make_shared<PlaceHolderType<T>>();
    add_helper<T>(std::move(config), placeholder);
    return placeholder;
}

template <typename T>
ConstPlaceHolder<std::vector<T>> Options::add_multivalent(Config<T> &&config) {
    auto placeholder = std::make_shared<PlaceHolderType<std::vector<T>>>();
    add_helper<T>(std::move(config), placeholder);
    return placeholder;
}

std::string Options::usage_string() const {
    auto usage_template = [](const auto &name) { return "[--" + name + "]"; };

    std::stringstream ss;
    for (const auto &pair : options_) {
        const auto &name = pair.first;
        const auto &option = pair.second;
        if (!option->positional()) {
            ss << usage_template(name) << " ";
        }
    }

    return ss.str();
}

std::string Options::display_string() const {
    OptionTable table({{"Required", "Name", "Letter", "Type", "Default", "Help", "Allowed Values"}});

    for (const auto &pair : options_) {
        const auto &option = pair.second;
        table.add_row(option->to_string());
    }

    return table.display();
}

std::shared_ptr<Option> Options::get(const std::string &name) {
    const bool is_letter = (name.length() == 1);

    if (is_letter) {
        for (const auto &pair : options_) {
            const auto &option = pair.second;
            if (option->letter() == name[0]) {
                return option;
            }
        }
    } else {
        const auto iterator = options_.find(name);
        if (iterator != options_.end()) {
            return iterator->second;
        }
    }

    return nullptr;
}

Options::NameLetterVector Options::check_requirements(const std::unordered_set<std::string> &existing_args) const {
    NameLetterVector missing{};
    for (const auto &pair : required_options_) {
        const auto &name = pair.first;
        const auto &letter = pair.second;
        const bool name_exists = existing_args.find(name) != existing_args.end();
        const bool letter_exists = existing_args.find(letter) != existing_args.end();
        if (!name_exists && !letter_exists) {
            missing.push_back(pair);
        }
    }

    return missing;
}

template <typename T, typename PlaceholderType>
void Options::add_helper(Config<T> &&config, PlaceholderType &placeholder) {
    const auto name = config.name;
    auto option = std::make_shared<Option>(placeholder, std::forward<Config<T>>(config));
    options_[name] = option;

    if (config.required) {
        const auto letter_to_str = std::string(1, config.letter);
        required_options_.push_back({name, letter_to_str});
    }
}

/// @{ Explicit Instantiation
template ConstPlaceHolder<std::string> Options::add(Config<std::string> &&);
template ConstPlaceHolder<double> Options::add(Config<double> &&);
template ConstPlaceHolder<float> Options::add(Config<float> &&);
template ConstPlaceHolder<uint64_t> Options::add(Config<uint64_t> &&);
template ConstPlaceHolder<int64_t> Options::add(Config<int64_t> &&);
template ConstPlaceHolder<uint32_t> Options::add(Config<uint32_t> &&);
template ConstPlaceHolder<int32_t> Options::add(Config<int32_t> &&);
template ConstPlaceHolder<uint16_t> Options::add(Config<uint16_t> &&);
template ConstPlaceHolder<int16_t> Options::add(Config<int16_t> &&);
template ConstPlaceHolder<uint8_t> Options::add(Config<uint8_t> &&);
template ConstPlaceHolder<int8_t> Options::add(Config<int8_t> &&);
template ConstPlaceHolder<bool> Options::add(Config<bool> &&);
template ConstPlaceHolder<char> Options::add(Config<char> &&);
template ConstPlaceHolder<std::vector<std::string>> Options::add_multivalent(Config<std::string> &&);
template ConstPlaceHolder<std::vector<double>> Options::add_multivalent(Config<double> &&);
template ConstPlaceHolder<std::vector<float>> Options::add_multivalent(Config<float> &&);
template ConstPlaceHolder<std::vector<uint64_t>> Options::add_multivalent(Config<uint64_t> &&);
template ConstPlaceHolder<std::vector<int64_t>> Options::add_multivalent(Config<int64_t> &&);
template ConstPlaceHolder<std::vector<uint32_t>> Options::add_multivalent(Config<uint32_t> &&);
template ConstPlaceHolder<std::vector<int32_t>> Options::add_multivalent(Config<int32_t> &&);
template ConstPlaceHolder<std::vector<uint16_t>> Options::add_multivalent(Config<uint16_t> &&);
template ConstPlaceHolder<std::vector<int16_t>> Options::add_multivalent(Config<int16_t> &&);
template ConstPlaceHolder<std::vector<uint8_t>> Options::add_multivalent(Config<uint8_t> &&);
template ConstPlaceHolder<std::vector<int8_t>> Options::add_multivalent(Config<int8_t> &&);
template ConstPlaceHolder<std::vector<bool>> Options::add_multivalent(Config<bool> &&);
template ConstPlaceHolder<std::vector<char>> Options::add_multivalent(Config<char> &&);
/// @}

} // namespace argparse
