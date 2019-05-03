#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <numeric>
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

    explicit Table(Row &&header) {
        rows_.push_back(std::move(header));
    }

    void add_row(Row &&row) {
        rows_.push_back(std::move(row));
    }

    const std::string &display(const bool reload = true) {
        if (reload) {
            table_.clear();
            generate();
        }

        return table_;
    }

  private:
    std::string table_;
    std::vector<Row> rows_;

    std::string align(const std::string &s, const std::size_t spaces) const {
        switch (RowAlignment) {
        case Alignment::Left:
            return s + std::string(spaces, ' ');
        case Alignment::Right:
            return std::string(spaces, ' ') + s;
        case Alignment::Center: {
            const std::size_t left_spaces = spaces / 2;
            const std::size_t right_spaces = spaces - left_spaces;
            return std::string(left_spaces, ' ') + s + std::string(right_spaces, ' ');
        }
        default:
            assert(false);
        }
    }

    void generate() {
        constexpr char kEndLine = '\n';

        std::array<std::size_t, Columns> lengths{};

        // Determine max lengths for each column
        for (const auto &row : rows_) {
            for (std::size_t col = 0; col < Columns; col++) {
                lengths[col] = std::max(lengths[col], row[col].length());
            }
        }

        constexpr std::size_t kNumSideVerticalColumns = 2;
        constexpr std::size_t kNumVerticalColumns = kNumSideVerticalColumns + (Columns - 1);
        const std::size_t total_width_of_columns = std::accumulate(lengths.begin(), lengths.end(), 0ULL);
        const std::size_t table_width = kNumVerticalColumns + total_width_of_columns;
        const std::string horizontal_border(table_width, '-');

        table_ += horizontal_border + kEndLine;

        for (std::size_t ii = 0; ii < rows_.size(); ii++) {
            const auto &row = rows_[ii];

            table_ += "|";
            for (std::size_t col = 0; col < Columns; col++) {
                const std::size_t remaining_spaces = lengths[col] - row[col].length();
                table_ += align(row[col], remaining_spaces);
                table_ += "|";
            }
            table_ += kEndLine;

            // After the header row, add a line to separate
            if (0 == ii) {
                table_ += horizontal_border + kEndLine;
            }
        }
        table_ += horizontal_border + kEndLine;
    }
};
