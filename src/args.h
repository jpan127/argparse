#pragma once

#include <string>
#include <unordered_map>
#include <vector>

/// Wrapper / interface over two maps
class Args {
    using Values = std::vector<std::string>;

  public:
    void create(const std::string &key) { string_map_[key]; }
    void create(const std::string &key, Values &&value) { string_map_[key] = std::move(value); }
    void insert(const std::string &key, std::string &&value) { string_map_[key].push_back(std::move(value)); }
    bool exists(const std::string &key) const { return string_map_.find(key) != string_map_.end(); }
    const Values &get(const std::string &key) const { return string_map_.at(key); }
    std::size_t size() const { return string_map_.size(); }

    auto begin() const { return string_map_.begin(); }
    auto end() const { return string_map_.end(); }
    auto begin() { return string_map_.begin(); }
    auto end() { return string_map_.end(); }

  private:
    std::unordered_map<std::string, Values> string_map_;
};
