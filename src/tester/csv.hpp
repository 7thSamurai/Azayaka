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

#pragma once

#include <string>
#include <vector>
#include <map>

class Csv
{
public:
    using Row = std::map<std::string, std::string>;
    using iterator = typename std::vector<Row>::iterator;
    using const_iterator = typename std::vector<Row>::const_iterator;

    Csv() = default;
    Csv(const std::string &path);

    bool open(const std::string &path);

    const Row &operator [] (std::size_t index) const;
    const Row &at(std::size_t index) const;

    std::size_t num_rows() const;
    bool col_exists(const std::string &name);

    // Iterators
    iterator begin() { return rows.begin(); }
    iterator end() { return rows.end(); }
    const_iterator begin() const { return rows.begin(); }
    const_iterator end() const { return rows.end(); }

private:
    std::vector<std::string> header;
    std::vector<Row> rows;
};

