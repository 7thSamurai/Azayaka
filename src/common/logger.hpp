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
        Debug,   // Verbose debugging information
        Error,   // Fatal Error
    };

    void log(const std::string &msg, Level level);

    void enable(bool enabled);
    void enable_verbose(bool enabled);

    // Singleton
    static Logger &get_instance();

private:
    bool enabled, verbose_enabled;
};

#define LOG_INFO(msg) \
    do { \
        Logger::get_instance().log(msg, Logger::Level::Info); \
    } while (0)

#define LOG_NOTICE(msg) \
    do { \
        Logger::get_instance().log(msg, Logger::Level::Notice); \
    } while (0)

#define LOG_WARNING(msg) \
    do { \
        Logger::get_instance().log(msg, Logger::Level::Warning); \
    } while (0)

#define LOG_DEBUG(msg) \
    do { \
        Logger::get_instance().log(msg, Logger::Level::Debug); \
    } while (0)

#define LOG_ERROR(msg) \
    do { \
        Logger::get_instance().log(msg, Logger::Level::Error); \
    } while (0)
