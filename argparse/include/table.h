#pragma once

#include <array>
#include <cassert>
#include <numeric>
#include <string>
#include <vector>

namespace argparse {

/// Specifies how values in columns should be aligned
enum class Alignment {
    Left,
    Center,
    Right,
};

/// Generates an aligned table
template <std::size_t Columns, Alignment RowAlignment = Alignment::Center>
class Table {
  public:
    using Row = std::array<std::string, Columns>;

    /// Tables should be initialized with a header row
    explicit Table(Row &&header);

    /// Adds a row of values
    void add_row(Row &&row);

    /// Converts the table into a string
    /// \param reload True to regenerate the table, false to use the last generated string
    const std::string &display(const bool reload = true);

  private:
    /// String version of the table
    std::string table_;

    /// Vector of rows
    std::vector<Row> rows_;

    /// Aligns the value by inserting spaces in the right places
    std::string align(const std::string &s, const std::size_t spaces) const;

    /// Generates the string version of the table
    void generate();
};

/// Implementation
#include "table.ipp"

} // namespace argparse
