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
#include "tester/csv_file.hpp"
#include "tester/suites/suite.hpp"

#include <chrono>
#include <iostream>

void Tester::add_suite(std::unique_ptr<TestSuite> suite) {
    suites.push_back(std::move(suite));
}

bool Tester::run(const std::string &results_csv) {
    // Attempt to load the correct results CSV
    CsvFile correct_results;
    if (!correct_results.open(results_csv)) {
        std::cerr << "Failed to load the Correct Results CSV" << std::endl;
        return false;
    }

    // Run each suite
    for (auto &suite : suites) {
        // Attempt to run the suite
        if (!suite->run(correct_results))
            return false;

    }

    return true;
}
