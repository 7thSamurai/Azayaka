// Copyright (C) 2020 Zach Collins
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

#include "csv.hpp"

#include <fstream>
#include <sstream>

Csv::Csv() {
    num_of_cols = 0;
}

int Csv::load(const std::string &file_path) {
    rows.clear();

    std::ifstream file(file_path);
    if (!file.is_open())
        return -1;

    std::string value;

    while (std::getline(file, value)) {
        CsvRow row;

        std::stringstream line;
        line << value;

        while (std::getline(line, value, ','))
            row.push_back(value);

        rows.push_back(row);
        num_of_cols = row.size();
    }

    file.close();

    return 0;
}

const CsvRow &Csv::operator [] (int index) const {
    return rows.at(index);
}

int Csv::find_header(const std::string &col_header) const {
    for (unsigned int i = 0; i < rows.front().size(); i++) {
        if (rows.front().at(i) == col_header)
            return i;
    }

    return -1;
}

int Csv::get_num_of_cols() const {
    return num_of_cols;
}

int Csv::get_num_of_rows() const {
    return rows.size();
}
