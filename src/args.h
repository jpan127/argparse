#pragma once

#include <string>
#include <unordered_map>
#include <vector>

/// Wrapper / interface over two maps
class Args {
  public:
    using Values = std::vector<std::string>;

    void create(const std::string &key) {
        string_map_[key];
    }

    void create(const char key) {
        letter_map_[key];
    }

    void create(const std::string &key, Values &&value) {
        string_map_[key] = std::move(value);
    }

    void create(const char key, Values &&value) {
        letter_map_[key] = std::move(value);
    }

    void insert(const std::string &key,std::string &&value) {
        string_map_[key].push_back(std::move(value));
    }

    void insert(const char key, std::string &&value) {
        letter_map_[key].push_back(std::move(value));
    }

    bool exists(const std::string &key) const {
        return string_map_.find(key) != string_map_.end();
    }

    bool exists(const char key) const {
        return letter_map_.find(key) != letter_map_.end();
    }

    const Values &get(const std::string &key) const {
        return string_map_.at(key);
    }

    const Values &get(const char key) const {
        return letter_map_.at(key);
    }

    std::size_t size() const {
        return letter_map_.size() + string_map_.size();
    }

    auto &letter_map() {
        return letter_map_;
    }

    auto &string_map() {
        return string_map_;
    }

  private:
    std::unordered_map<char, Values> letter_map_;
    std::unordered_map<std::string, Values> string_map_;
};
