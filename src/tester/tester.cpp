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
#include <fstream>
#include <iomanip>

void Tester::add_suite(std::unique_ptr<TestSuite> suite) {
    suites.push_back(std::move(suite));
}

bool Tester::run(const std::string &results_csv) {
    std::cout << "\r\033[1;92m[======]\033[0m Starting Azayaka Tester..." << std::endl;
    std::cout << "\r\033[1;92m[======]\033[0m Copyright (C) 2020-2022 Zach Collins" << std::endl;

    // Attempt to load the correct results CSV
    CsvFile correct_results;
    if (!correct_results.open(results_csv)) {
        std::cerr << "Failed to load the CSV file " << results_csv << std::endl;
        return false;
    }

    // Make sure that all the needed columns are present in the CSV
    if (!correct_results.col_exists("type")) {
        std::cerr << "Missing column 'name' in CSV" << std::endl;
        return false;
    }
    if (!correct_results.col_exists("name")) {
        std::cerr << "Missing column 'name' in CSV" << std::endl;
        return false;
    }
    if (!correct_results.col_exists("crc32")) {
        std::cerr << "Missing column 'name' in CSV" << std::endl;
        return false;
    }

    auto start = std::chrono::steady_clock::now();

    // Keep track of how many tests passed and failed
    unsigned int passed = 0;
    unsigned int failed = 0;

    // Run each suite
    for (auto &suite : suites) {
        // Attempt to run the suite
        if (!suite->run(correct_results))
            return false;

        // Count the results
        for (const auto &result : suite->results()) {
            if (result.passed)
                passed++;
            else
                failed++;
        }
    }

    // Find the time it took this test to run
    auto end  = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    std::cout << "\n\033[1;92m[======]\033[0m Ran " << (passed+failed) << " tests (" << time.count() << " sec)" << std::endl;

    if (failed == 0)
        std::cout << "\033[1;91m[------]\033[0m ";
    else
        std::cout << "\033[1;91m[------]\033[0m ";

    std::cout << passed << "/" << (passed+failed) << " passed (" << failed << " failures)" << std::endl;

    return true;
}

bool Tester::gen_results(const std::string &path) {
    std::cout << "\r\033[1;92m[------]\033[0m Outputting Markdown results to " << path << std::endl;

    // Open the output file
    std::ofstream file(path);
    if (!file.is_open())
        return false;

    file << "# Test Results" << std::endl;

    // Go through each suite
    for (auto &suite : suites) {
        // Skip any empty results
        auto results = suite->results();
        if (results.empty())
            continue;

        // Output the suite section header
        file << "\n## " << suite->title() << "\n" << std::endl;

        // Count how many tests passed total
        unsigned int total_passed = 0;
        for (const auto &result : results) {
            if (result.passed)
                total_passed++;
        }

        // Output the ratio of the tests that passed
        file << "**" << total_passed << "/" << results.size() << " passed**\n" << std::endl;

        // Output the result table header
        file << "| Test | Result |" << std::endl;
        file << "| --- | --- |" << std::endl;

        // Print out each result row in the table
        for (const auto &result : results) {
            if (result.passed)
                file << "| " + result.name + " | :+1: |" << std::endl;
            else
                file << "| " + result.name + " | :x: |" << std::endl;
        }
    }

    // Output the time and date that this file was generated at
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    file << "\nAutomatically generated at **" << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X") << "**" << std::endl;

    file.close();

    return true;
}
