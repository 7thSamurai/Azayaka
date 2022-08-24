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

#include "core/rom/rtc.hpp"
#include "core/defs.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"
#include "common/binary_file.hpp"

#include <ctime>

Rtc::Rtc() {
    timer = 0;
    last_time = time(NULL);

    seconds = 0;
    minutes = 0;
    hours   = 0;
    days    = 0;

    latch_value = 0;

    halt = 0;
    overflow = 0;
}

void Rtc::latch(byte value) {
    // Latch the time into the registers
    if (latch_value == 0 && value == 1) {
        update();

        seconds = timer % 60;
        minutes = (timer / 60) % 60;
        hours   = (timer / 3600) % 24;
        days    = (timer / 3600 / 24) & 511;
    }

    latch_value = value;
}

byte Rtc::get_reg(byte reg) {
    switch (reg) {
        case 0x08: // Sec
            return seconds;
        case 0x09: // Min
            return minutes;
        case 0x0A: // Hour
            return hours;
        case 0x0B: // Day - low
            return days & 0xFF;
        case 0x0C: // Day - high
            check_overflow();
            return (days >> 8) | (halt ? BIT6 : 0) | (overflow ? BIT7 : 0);

        default:
            break;
    }

    LOG_WARNING("Rtc::get_reg can't handle register 0x" + hex(reg));

    return 0;
}

void Rtc::set_reg(byte reg, byte value) {
    if (reg >= 0x08 && reg <= 0x0B && !halt)
        return;

    switch (reg) {
        case 0x08: // Sec
            timer -= int(timer % 60) - int(value);
            return;

        case 0x09: // Min
            timer -= (int((timer / 60) % 60) - int(value)) * 60;
            return;

        case 0x0A: // Hour
            timer -= (int((timer / 3600) % 24) - int(value)) * 3600;
            return;

        case 0x0B: // Day - low
            timer -= (int((timer / 3600) / 24) - int(value)) * 3600 * 24;
            return;

        case 0x0C: // Day - high
            if (!halt)
                timer -= (int((timer / 3600) / 24) - int((value & 1) << 8)) * 3600 * 24;

            halt_timer(value & BIT6);
            overflow = value & BIT7;
            return;

        default:
            break;
    }

    LOG_WARNING("Rtc::set_reg can't handle register 0x" + hex(reg));
}

int Rtc::load(const std::string &file_path) {
    BinaryFile file(file_path, BinaryFile::Mode_Read);

    if (!file.is_open())
        return -1;

    timer     = file.read64();
    last_time = file.read64();

    return 0;
}

int Rtc::save(const std::string &file_path) {
    update();

    BinaryFile file(file_path, BinaryFile::Mode_Write);

    if (!file.is_open())
        return -1;

    file.write64(timer);
    file.write64(last_time);

    return 0;
}

void Rtc::halt_timer(bool value) {
    if (!halt && value)
        update();

    else if (halt && !value)
        last_time = time(NULL);

    halt = value;
}

void Rtc::update() {
    if (halt)
        return;

    s64 t = time(NULL);

    timer += t - last_time;
    last_time = t;
}

void Rtc::check_overflow() {
    int days = timer / 3600 / 24;

    if (days > 511) {
        timer -= (512) * 3600 * 24;

        overflow = 1;
    }
}
