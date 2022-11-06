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

#include "tester/suites/suite.hpp"
#include "core/gameboy.hpp"

#include <iostream>
#include <filesystem>
#include <chrono>
#include <algorithm>

TestSuite::TestSuite(const std::string &base_path) {
    this->base_path = base_path;
}

std::vector<std::pair<std::string, bool>> TestSuite::run() {
    // Grab the start time
    auto start = std::chrono::steady_clock::now();

    // Generate the path of the test suite
    auto path = std::filesystem::path(base_path + "/" + name());

    // Make sure that the test suite directory exists
    if (!std::filesystem::exists(path)) {
        std::cerr << "Error: Missing " << name() << " directory in " << base_path << std::endl;
        return {};
    }

    // And that it is actually a directory
    if (!std::filesystem::is_directory(path)) {
        std::cerr << "Error: " << path.string() << " is not a directory" << std::endl;
        return {};
    }

    // Find the ROMs
    std::vector<std::string> roms;
    for (const auto &entry : std::filesystem::recursive_directory_iterator(path)) {
        if (!entry.is_regular_file())
            continue;

        // Only allow .gb or .gbc ROM files
        if (entry.path().extension() == ".gb" || entry.path().extension() == ".gbc")
            roms.push_back(entry.path().string());
    }

    // Make sure that some ROMs were found
    if (roms.empty()) {
        std::cerr << "Error: No ROMs were found in " << path << std::endl;
        return {};
    }

    // Sort the ROMS alphabetically
    std::sort(roms.begin(), roms.end());

    std::vector<std::pair<std::string, bool>> results;
    unsigned int pass_count = 0;

    std::cout << "Running " << roms.size() << " tests from " << name() << std::endl;

    // Go through each ROM
    for (const auto &rom_path : roms) {
        auto start = std::chrono::steady_clock::now();

        std::string error;
        GameBoy gb;

        // Attempt to load the ROM
        if (gb.load_rom(rom_path, error) < 0) {
            std::cerr << "Error: Unable to load ROM " << rom_path << std::endl;
            return {};
        }

        // Now run the Game Boy until the test is complete
        gb.init();
        run_until_done(gb);

        // Check if the test passed
        if (1) { // TODO
            std::cout << "[PASSED]";
            results.push_back(std::make_pair(rom_path, true));
            pass_count++;
        }
        else {
            std::cout << "[FAILED]";
            results.push_back(std::make_pair(rom_path, false));
        }

        // Find the time it took this test to run
        auto end  = std::chrono::steady_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << rom_path << " (" << time.count() << " msec)" << std::endl;
    }

    // Find the total time it took this suite to run
    auto end  = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    // Print out status information
    std::cout << "Ran " << roms.size() << " tests (" << time.count() << " sec)" << std::endl;
    std::cout << pass_count << "/" << roms.size() << " passed" << std::endl;

    return results;
}

