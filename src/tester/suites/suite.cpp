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
#include "tester/csv_file.hpp"
#include "core/gameboy.hpp"
#include "core/cpu/cpu.hpp"
#include "common/hash.hpp"
#include "common/image.hpp"

#include <iostream>
#include <filesystem>
#include <chrono>
#include <algorithm>

TestSuite::TestSuite(const std::string &base_path) {
    this->base_path = base_path;
}

bool TestSuite::run(const CsvFile &correct_results) {
    // Grab the start time
    auto start = std::chrono::steady_clock::now();

    // Generate the path of the test suite
    auto suite_path = std::filesystem::path(base_path + "/" + name());
    auto results_path = base_path + "/results/" + name();

    // Make sure that the test suite directory exists
    if (!std::filesystem::exists(suite_path)) {
        std::cerr << "Error: Missing " << name() << " directory in " << base_path << std::endl;
        return false;
    }

    // And that it is actually a directory
    if (!std::filesystem::is_directory(suite_path)) {
        std::cerr << "Error: " << suite_path.string() << " is not a directory" << std::endl;
        return false;
    }

    // Create the results directory
    if (!std::filesystem::create_directories(std::filesystem::path(results_path))) {
        std::cerr << "Unable to create the results directory at " << results_path << std::endl;
        return false;
    }

    // Find the ROMs
    std::vector<std::string> roms;
    for (const auto &entry : std::filesystem::recursive_directory_iterator(suite_path)) {
        if (!entry.is_regular_file())
            continue;

        // Only allow .gb or .gbc ROM files
        if (entry.path().extension() == ".gb" || entry.path().extension() == ".gbc")
            roms.push_back(entry.path().string());
    }

    // Make sure that some ROMs were found
    if (roms.empty()) {
        std::cerr << "Error: No ROMs were found in " << suite_path << std::endl;
        return false;
    }

    // Sort the ROMS alphabetically
    std::sort(roms.begin(), roms.end());

    std::cout << "Running " << roms.size() << " tests from " << name() << std::endl;

    // Go through each ROM
    unsigned int pass_count = 0;
    for (const auto &rom_path : roms) {
        auto start = std::chrono::steady_clock::now();
        auto name  = rom_path.substr(suite_path.string().size());

        std::string error;
        GameBoy gb;

        // Attempt to load the ROM
        if (gb.load_rom(rom_path, error) < 0) {
            std::cerr << "Error: Unable to load ROM " << rom_path << std::endl;
            return false;
        }

        // Now run the Game Boy until the test is complete
        gb.init();
        while (!done(gb)) {
            gb.cpu->step();
        }

        // Make sure that the screen is fully rendered
        gb.run_frame();

        // Get the checksum of the old screenshot
        auto bmp_path = results_path + std::filesystem::path(rom_path.substr(suite_path.string().size())).replace_extension().string() + ".bmp";
        auto old_checksum = Common::crc32(bmp_path);

        // Save a screenshot of the test results
        if (Common::save_bmp(bmp_path, gb.get_screen_buffer(), 160, 144) < 0) {
            std::cerr << "Failed to save BMP " << bmp_path << std::endl;
            return false;
        }

        // Find the correct checksum
        auto it = correct_results.find_row("name", name);
        if (it == correct_results.end()) {
            std::cerr << "Missing entry in CSV for " << name << std::endl;
            return false;
        }

        // Check if the test passed
        auto new_checksum = Common::crc32(bmp_path);
        bool passed = new_checksum == std::stoul(it->find("crc32")->second);

        // Generate the result
        Result result;
        result.name    = name;
        result.changed = new_checksum != old_checksum;
        result.passed  = passed;

        // Check if the test passed
        if (passed) {
            std::cout << "[PASSED] ";
            pass_count++;
        }
        else {
            std::cout << "[FAILED] ";
        }

        // Add the result
        results_.push_back(result);

        // Find the time it took this test to run
        auto end  = std::chrono::steady_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << name << " (" << time.count() << " msec)" << std::endl;
    }

    // Find the total time it took this suite to run
    auto end  = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    // Print out status information
    std::cout << "Ran " << roms.size() << " tests (" << time.count() << " sec)" << std::endl;
    std::cout << pass_count << "/" << roms.size() << " passed" << std::endl;

    return true;
}

const std::vector<TestSuite::Result> &TestSuite::results() const {
    return results_;
}
