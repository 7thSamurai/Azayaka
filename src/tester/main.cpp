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

#include "tester/tester.hpp"
#include "tester/suite.hpp"
#include "common/logger.hpp"
#include "common/string_utils.hpp"
#include <iostream>
#include <filesystem>

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <Test ROMs Directory> [Suites...]" << std::endl;
        return -1;
    }

    // Make sure that the given test directory is valid
    std::string test_dir = argv[1];
    if (!std::filesystem::is_directory(std::filesystem::path(test_dir))) {
        std::cerr << "Invalid directory " << test_dir << std::endl;
        return -1;
    }

    // Disable the logger so it isn't printing out a bunch of garbage we don't want
    Logger::get_instance().enable(0);

    // Create the tester and add the individual test suites
    Tester tester;
    tester.add_suite(std::make_unique<BlarggSuite> (test_dir));
    tester.add_suite(std::make_unique<MooneyeSuite>(test_dir));

    // If any specific suites were requested, generate a list of those
    std::vector<std::string> suites;
    for (int i = 2; i < argc; i++)
        suites.push_back(StringUtils::to_lower(argv[i]));

    // Run the tester
    auto csv_path = test_dir + "/test_results.csv";
    if (!tester.run(csv_path, suites)) {
        return -1;
    }

    if (!tester.gen_results(test_dir + "/results/TestResults.md")) {
        std::cerr << "Unable to save results!" << std::endl;
        return -1;
    }

    return 0;
}
