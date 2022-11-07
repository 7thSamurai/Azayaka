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
#include <memory>

class TestSuite;

class Tester
{
public:
    Tester() = default;
    ~Tester() = default;

    // Adds a test suite to the tester
    void add_suite(std::unique_ptr<TestSuite> suite);

    // Runs all the different test suites
    bool run(const std::string &results_csv, const std::vector<std::string> &suites);

    // Generate a markdown file of the test results at the given path
    bool gen_results(const std::string &path);

private:
    std::vector<std::unique_ptr<TestSuite>> suites;
};
