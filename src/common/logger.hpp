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

class Logger
{
public:
    Logger();
    ~Logger();

    enum Level {
        Info,    // General information
        Notice,  // Important Notice
        Warning, // Warning
        Error,   // Fatal Error
    };

    enum Verbosity {
        NonVerbose = 0,
        Verbose    = 1
    };

    void log(const std::string &msg, Level level, Verbosity verbosity = NonVerbose);

    void enable(bool enabled);
    void enable_verbose(bool enabled);

private:
    bool enabled, verbose_enabled;
};
