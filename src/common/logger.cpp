// Copyright (C) 2020-2021 Zach Collins <the_7thSamurai@protonmail.com>
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

#include "common/logger.hpp"

#include <iostream>

Logger::Logger() {
    enabled = 1;
    verbose_enabled = 0;
}

Logger::~Logger() {
}
// verbose verbosity
void Logger::log(const std::string &msg, Level level) {
    if (!enabled)
        return;

    if (level == Level::Debug && !verbose_enabled)
        return;

#ifdef __linux__
    switch (level) {
        case Level::Info:
        case Level::Debug:
            std::cout << msg << std::endl;
            break;

        case Level::Notice:
            std::cout << "\033[1;92m" << msg << "\033[0m" << std::endl;
            break;

        case Level::Warning:
            std::cout << "\033[1;93m" << msg << "\033[0m" << std::endl;
            break;

        case Level::Error:
            std::cout << "\033[1;91m" << msg << "\033[0m" << std::endl;
            break;
    }
#else
    std::cout << msg << std::endl;
#endif
}

void Logger::enable(bool enabled) {
    this->enabled = enabled;
}

void Logger::enable_verbose(bool enabled) {
    verbose_enabled = enabled;
}

Logger &Logger::get_instance() {
    // Singleton
    static Logger logger;

    return logger;
}
