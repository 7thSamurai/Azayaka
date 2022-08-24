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

#include "tester/csv.hpp"
#include "core/types.hpp"

#include <vector>

class Results
{
public:
    Results();

    int init(const std::string &csv_path);

    void add_result(const std::string &rom_name, u32 checksum);
    int save_results(const std::string &file_path);

private:
    Csv csv;

    std::vector <std::string> blargg_results;
    std::vector <std::string> mooneye_results;

    int blargg_passed, mooneye_passed;

    int type_col, name_col, crc32_col;
};
