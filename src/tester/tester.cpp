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
#include <chrono>

void Tester::add_suite(std::unique_ptr<TestSuite> suite) {
    suites.push_back(std::move(suite));
}

bool Tester::run() {
    bool success = true;

    // Run each suite
    for (auto &suite : suites) {
        auto results = suite->test();

        // Check if the suite passsed
        if (results.empty())
            success = true;
    }

    return success;
}
