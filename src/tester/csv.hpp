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

#ifndef _CSV_HPP_
#define _CSV_HPP_

#include <string>
#include <vector>

typedef std::vector <std::string> CsvRow;

class Csv
{
public:
    Csv();

    int load(const std::string &file_path);

    const CsvRow &operator [] (int index) const;

    int find_header(const std::string &col_header) const;

    int get_num_of_cols() const;
    int get_num_of_rows() const;

private:
    std::vector <CsvRow> rows;

    int num_of_cols;
};

#endif // _CSV_HPP_
