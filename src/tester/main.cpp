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
#include "tester/suites/suite.hpp"
#include "common/logger.hpp"
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <Test ROMs Directory>" << std::endl;
        return -1;
    }

    // Disable the logger so it isn't printing out a bunch of garbage we don't want
    Logger::get_instance().enable(0);

    // Create the tester and add the individual test suites
    Tester tester;
    tester.add_suite(std::make_unique<BlarggSuite> (argv[1]));
    tester.add_suite(std::make_unique<MooneyeSuite>(argv[1]));

    // Run the tester
    auto csv_path = std::string(argv[1]) + "/test_results.csv";
    if (!tester.run(csv_path)) {
        return -1;
    }

    if (!tester.gen_results("TestResults.md")) {
        std::cout << "Unable to save results!" << std::endl;
        return -1;
    }

    return 0;
}
