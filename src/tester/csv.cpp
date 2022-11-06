// Copyright (C) 2020-2022 Zach Collins <the_7thSamurai@protonmail.com>
//
// Azayaka is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Azayaka is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Azayaka. If not, see <https://www.gnu.org/licenses/>.

#include "tester/csv.hpp"

#include <fstream>
#include <sstream>
#include <optional>
#include <algorithm>

Csv::Csv(const std::string &path) : Csv() {
    open(path);
}

bool Csv::open(const std::string &path) {
    // Open the file
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    // Reads a line and returns the tokenized contents
    auto parse_line = [&]() -> std::optional<std::vector<std::string>> {
        // Read the line
        std::string str;
        if (!std::getline(file, str))
            return {};

        // Tokenize the line
        std::stringstream ss;
        ss << str;

        // Split at any colons
        std::vector<std::string> tokens;
        while (std::getline(ss, str, ','))
            tokens.push_back(str);

        return std::optional<std::vector<std::string>>(tokens);
    };

    // Read the header
    auto value = parse_line();
    if (!value)
        return false;

    header = *value;

    // Parse the rest of the values
    while (value = parse_line()) {
        // Make sure that this row isn't bigger than the header row
        if (value->size() > header.size())
            return false;

        // Populate the row
        Row row;
        for (std::size_t i = 0; i < value->size(); i++)
            row.insert(std::make_pair(header.at(i), value->at(i)));

        // Add the row
        rows.push_back(row);
    }

    file.close();

    return true;
}

const Csv::Row &Csv::operator [] (std::size_t index) const {
    return rows.at(index);
}

const Csv::Row &Csv::at(std::size_t index) const {
    return rows.at(index);
}

std::size_t Csv::num_rows() const {
    return rows.size();
}

bool Csv::col_exists(const std::string &name) {
    return std::find(header.begin(), header.end(), name) != header.end();
}
