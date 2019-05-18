template <std::size_t Columns, Alignment RowAlignment>
Table<Columns, RowAlignment>::Table(Row &&header) {
    rows_.push_back(std::move(header));
}

template <std::size_t Columns, Alignment RowAlignment>
void Table<Columns, RowAlignment>::add_row(Row &&row) {
    rows_.push_back(std::move(row));
}

template <std::size_t Columns, Alignment RowAlignment>
const std::string &Table<Columns, RowAlignment>::display(const bool reload) {
    if (reload) {
        table_.clear();
        generate();
    }

    return table_;
}

template <std::size_t Columns, Alignment RowAlignment>
std::string Table<Columns, RowAlignment>::align(const std::string &s, const std::size_t spaces) const {
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

template <std::size_t Columns, Alignment RowAlignment>
void Table<Columns, RowAlignment>::generate() {
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
