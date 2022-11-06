# Tester

Azayaka has an automatic tester that runs the blargg and mooneye test rom suites.

You can view the test results [here](TestResults.md)

## Description

The tester scans a directory for test roms. It then runs each one of those roms until an infinite loop is detected, signaling that the test has finished. It then saves a screenshot as a BMP image, that way you can reference the result later. It generates a checksum of the old screenshot and the new screenshot. If those differ it alerts you that the result of that test has changed. It also compares the new checksum to the corresponding checksum in the test results CSV file to determine if the test passed or not. It then automatically generates a markdown file containing a table of every test and result.

## Acquiring the test Roms

The Blargg test suite can be downloaded [here](https://gbdev.gg8.se/files/roms/blargg-gb-tests/), and the Mooneye test suite can be downloaded [here](https://gekkio.fi/files/mooneye-test-suite/).

A copy of all the tests are also included in the [tests directory](tests/) of this repo.

## Setup

There needs to be a folder which contains directories called "blargg" and "mooneye" which contain the respective roms.

There also need to be a .csv file which contains the correct results for each rom.

Note: Be sure to remove the "utils" folder in the Mooneye tests. Otherwise the tester will freeze. 

## Test Results CSV

The first row must contain columns called "type", "name" and "crc32". These can be in any order you desire.

Starting at the second row, every row contains an entry for each rom. The "name" column must contain the name of the rom. The "type" column must be "blargg" or "mooneye". The "crc32" column contains the crc32 checksum of the correct result.

Note: Azayaka comes with such a CSV file, which is named "TestResults.csv".


