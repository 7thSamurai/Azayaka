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

#include "gameboy.hpp"
#include "results.hpp"
#include "globals.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"
#include "common/hash.hpp"
#include "common/image.hpp"
#include "common/file_utils.hpp"

#include <filesystem>
#include <iostream>
#include <ctime>
#include <algorithm>

enum RomType {
    RomType_Blargg,
    RomType_Mooneye,
};

void test_rom(const std::string &base_path, RomType type);

std::string sec_to_time(int time);

Results results;

const char status_wheel[] = {
    '|', '/', '-', '|', '/', '-', '\\'
};

unsigned int status_wheel_pos = 0;

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <Test ROMs Path> <CSV Path>" << std::endl;
        return -1;
    }

    if (results.init(std::string(argv[2])) < 0)
        std::cout << "Unable to load CSV!" << std::endl;

    logger.enable(0);

    time_t time_start = time(NULL);

    std::cout << "Running Blargg tests" << std::endl;
    test_rom(std::string(argv[1])+"/blargg", RomType_Blargg);

    std::cout << "Running Mooneye tests" << std::endl;
    test_rom(std::string(argv[1])+"/mooneye", RomType_Mooneye);

    time_t time_end = time(NULL);
    std::cout << "All tests finished in " << sec_to_time(time_end-time_start) << std::endl;

    if (results.save_results("TestResults.md") < 0)
        std::cout << "Unable to save results!" << std::endl;

    return 0;
}

void test_rom(const std::string &base_path, RomType type) {
    status_wheel_pos = 0;

    int path_start = base_path.size()+1;

    time_t time_start = time(NULL);

    std::string path, bmp_path, error;
    uint32_t old_checksum, new_checksum;

    std::vector <std::string> roms;

    // Find the ROMs
    for (const auto &entry : std::filesystem::recursive_directory_iterator(base_path)) {
        if (!entry.is_regular_file())
            continue;

        if (entry.path().extension() == ".gb" || entry.path().extension() == ".gbc")
            roms.push_back(entry.path().string());
    }

    std::sort(roms.begin(), roms.end());

    for (unsigned int i = 0; i < roms.size(); i++) {
        path = roms.at(i);
        GameBoy gb;

        if (gb.load_rom(path, error) < 0) {
            std::cout << "Unable to load " << path << std::endl;
            continue;
        }

        gb.init();

        switch (type) {
            case RomType_Blargg:
                gb.run_until_blargg_done();
                break;

            case RomType_Mooneye:
                gb.run_until_mooneye_done();
                break;
        }

        bmp_path = File::remove_extension(path) + ".bmp";

        old_checksum = Common::crc32(bmp_path);
        Common::save_bmp(bmp_path, gb.get_screen_buffer(), 160, 144);
        new_checksum = Common::crc32(bmp_path);

        if (old_checksum != new_checksum)
            std::cout << "\"" << File::remove_extension(bmp_path) << "\" changed! " << hex(new_checksum, 8) << std::endl;

        results.add_result(path.substr(path_start), new_checksum);
    }

    time_t time_end = time(NULL);
    std::cout << "Finished in " << sec_to_time(time_end-time_start) << "\n" << std::endl;
}

std::string sec_to_time(int time) {
    int min = time / 60;
    int sec = time % 60;

    std::string time_str;

    if (min != 0)
        time_str += std::to_string(min) + " min ";
    if (sec != 0)
        time_str += std::to_string(sec) + " sec";

    return time_str;
}
