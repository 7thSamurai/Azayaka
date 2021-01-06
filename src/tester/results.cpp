// Copyright (C) 2020-2021 Zach Collins <the_7thSamurai@protonmail.com>
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

#include "results.hpp"
#include "common/utils.hpp"

#include <fstream>
#include <iostream>
#include <algorithm>

Results::Results() {
    type_col = name_col = crc32_col = 0;

    blargg_passed = mooneye_passed = 0;
}

int Results::init(const std::string &csv_path) {
    if (csv.load(csv_path) < 0)
        return -1;

    type_col  = csv.find_header("type");
    name_col  = csv.find_header("name");
    crc32_col = csv.find_header("crc32");

    return 0;
}

void Results::add_result(const std::string &rom_name, u32 checksum) {
    int row = -1;

    for (int i = 0; i < csv.get_num_of_rows(); i++) {
        if (csv[i][name_col] == rom_name) {
            row = i;
            break;
        }
    }

    if (row < 0) {
        std::cout << "Warning: \"" << rom_name << "\" is not in CSV!" << std::endl;
        return;
    }

    std::string type = csv[row][type_col];
    uint32_t crc32 = std::stol(csv[row][crc32_col], 0, 16);

    bool passed = crc32 == checksum;
    std::string result;

    if (passed)
        result = "| " + rom_name + " | :+1: |";
    else
        result = "| " + rom_name + " | :x: |";

    if (type == "blargg") {
        blargg_results.push_back(result);

        if (passed)
            blargg_passed++;
    }

    else if (type == "mooneye") {
        mooneye_results.push_back(result);

        if (passed)
            mooneye_passed++;
    }
}

int Results::save_results(const std::string &file_path) {
    std::sort(blargg_results.begin(), blargg_results.end());
    std::sort(mooneye_results.begin(), mooneye_results.end());

    std::ofstream file(file_path);
    if (!file.is_open())
        return -1;

    file << "# Test Results" << std::endl;

    file << "\n## Blargg's Tests\n" << std::endl;
    file << "**" << blargg_passed << "/" << blargg_results.size() << " passed**\n" << std::endl;
    file << "| Test | Result |" << std::endl;
    file << "| --- | --- |" << std::endl;

    for (const std::string &result : blargg_results)
        file << result << std::endl;

    file << "\n## Mooneye GB Tests\n" << std::endl;
    file << "**" << mooneye_passed << "/" << mooneye_results.size() << " passed**\n" << std::endl;
    file << "| Test | Result |" << std::endl;
    file << "| --- | --- |" << std::endl;

    for (const std::string &result : mooneye_results)
        file << result << std::endl;

    file.close();

    return 0;
}
