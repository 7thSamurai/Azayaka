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

#include "core/types.hpp"

#include <string>

class Rtc
{
public:
    Rtc();

    void latch(byte value);

    byte get_reg(byte reg);
    void set_reg(byte reg, byte value);

    int load(const std::string &file_path);
    int save(const std::string &file_path);

private:
    void halt_timer(bool value);
    void update();
    void check_overflow();

    s64 timer;
    s64 last_time;

    byte seconds;
    byte minutes;
    byte hours;
    word days; // This is only 9-bits

    byte latch_value;

    bool halt;
    bool overflow;
};
