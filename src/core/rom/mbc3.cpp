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

#include "core/rom/mbc3.hpp"
#include "core/defs.hpp"
#include "core/state.hpp"
#include "common/logger.hpp"
#include "common/string_utils.hpp"

Mbc3::Mbc3(int rom_size_num, int ram_size_num) : Mbc(rom_size_num, ram_size_num) {
}

unsigned int Mbc3::rom_size() const {
    return (0x4000 * 128);
}

unsigned int Mbc3::ecart_size() const {
    return (4 * 0x2000);
}

byte Mbc3::read_byte(word address, UsageType usage) {
    if (address <= 0x3FFF) {
        rom_usage[address] = usage;
        return data[address];
    }

    else if (address <= 0x7FFF) {
        rom_usage  [rom_offset + (address - 0x4000)] = usage;
        return data[rom_offset + (address - 0x4000)];
    }

    else if (address >= 0xA000 && address <= 0xBFFF) {
        if (ram_on) {
            if (ram_bank <= 0x03) {
                if (rom_type == 0x10 || rom_type == 0x12 || rom_type == 0x13) {
                    ram_offset = ram_bank * 0x2000;
                    return ecart[ram_offset + (address - 0xA000)];
                }
            }

            else if (ram_bank >= 0x08 && ram_bank <= 0x0C) {
                if (rom_type == 0x0F || rom_type == 0x10)
                    return rtc.get_reg(ram_bank);
            }
        }
    }

    else
        LOG_WARNING("Mbc3::read_byte can't access address 0x" + hex(address));

    return 0;
}

void Mbc3::write_byte(word address, byte value) {
    // RAM and Timer Enable
    if (address <= 0x1FFF) {
        if (rom_type == 0x0F || rom_type == 0x10 || rom_type == 0x12 || rom_type == 0x13)
            ram_on = ((value & 0x0F) == 0x0A) ? 1 : 0;
    }

    // ROM Bank Number
    else if(address <= 0x3FFF) {
        rom_bank = value & 0x7F;
        if (!rom_bank) rom_bank = 0x01;
        rom_offset = rom_bank * 0x4000;
    }

    // RAM Bank Number - or - RTC Register Select
    else if(address <= 0x5FFF) {
        if (rom_type == 0x0F || rom_type == 0x10 || rom_type == 0x12 || rom_type == 0x13)
            ram_bank = value;
    }

    // Latch Clock Data
    else if(address <= 0x7FFF) {
        if (rom_type == 0x0F || rom_type == 0x10)
            rtc.latch(value);
    }

    else if (address >= 0xA000 && address <= 0xBFFF) {
        if (ram_on) {
            if (ram_bank <= 0x03) {
                if (rom_type == 0x10 || rom_type == 0x12 || rom_type == 0x13) {
                    ram_offset = ram_bank * 0x2000;
                    ecart[ram_offset + (address - 0xA000)] = value;
                }
            }

            else if ((ram_bank >= 0x08) && (ram_bank <= 0x0C)) {
                if (rom_type == 0x0F || rom_type == 0x10)
                    rtc.set_reg(ram_bank, value);
            }
        }
    }

    else
        LOG_WARNING("Mbc3::write_byte can't access address 0x" + hex(address));
}

int Mbc3::load_timer(const std::string &file_path) {
    return rtc.load(file_path);
}

int Mbc3::save_timer(const std::string &file_path) {
    return rtc.save(file_path);
}
