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
class CsvFile;

// Base test suite
class TestSuite
{
public:
    // Test Results data structure
    struct Result {
        std::string name;
        bool changed;
        bool passed;
    };

    TestSuite(const std::string &base_path);
    virtual ~TestSuite() = default;

    // Run the test suite (Returns false if there are any errors getting the ROMs to run)
    bool run(const CsvFile &correct_results);

    // Get the test results
    const std::vector<Result> &results() const;

    // Get the name of the test suite
    virtual const char *name() const = 0;

    // Get the title of the test suite
    virtual const char *title() const = 0;

protected:
    // Checks if the test has finished
    virtual bool done(GameBoy &gb) = 0;

    std::string base_path;

private:
    std::vector<Result> results_;
};

// Blargg's tests
class BlarggSuite : public TestSuite
{
public:
    using TestSuite::TestSuite;

    const char *name () const override;
    const char *title() const override;

protected:
    bool done(GameBoy &gb) override;
};

// Gekkio's Mooneye Test Suite
class MooneyeSuite : public TestSuite
{
public:
    using TestSuite::TestSuite;

    const char *name () const override;
    const char *title() const override;

protected:
    bool done(GameBoy &gb) override;
};
