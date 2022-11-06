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

#include <string>
#include <vector>

class GameBoy;

// Base test suite
class TestSuite
{
public:
    TestSuite(const std::string &base_path);
    virtual ~TestSuite() = default;

    // Run the test suite
    std::vector<std::pair<std::string, bool>> run();

    // Get the name of the test suite
    virtual const char *name() const = 0;

protected:
    // Runs the Game Boy until the test has finished
    virtual void run_until_done(GameBoy &gb) = 0;

    std::string base_path;
};

// Blargg's tests
class BlarggSuite : public TestSuite
{
public:
    using TestSuite::TestSuite;

    const char *name() const override;

protected:
    void run_until_done(GameBoy &gb) override;
};

// Gekkio's Mooneye Test Suite
class MooneyeSuite : public TestSuite
{
public:
    using TestSuite::TestSuite;

    const char *name() const override;

protected:
    void run_until_done(GameBoy &gb) override;
};
