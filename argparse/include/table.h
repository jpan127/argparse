#pragma once

#include <array>
#include <string>
#include <vector>

enum class Alignment {
    Left,
    Center,
    Right,
};

template <std::size_t Columns, Alignment RowAlignment = Alignment::Center>
class Table {
  public:
    using Row = std::array<std::string, Columns>;

    explicit Table(Row &&header);

    void add_row(Row &&row);

    const std::string &display(const bool reload = true);

  private:
    std::string table_;
    std::vector<Row> rows_;

    std::string align(const std::string &s, const std::size_t spaces) const;

    void generate();
};

/// Implementation
#include "table.ipp"
