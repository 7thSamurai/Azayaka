// Copyright (C) 2020 Zach Collins
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

#include "mbc5.hpp"
#include "globals.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"

Mbc5::Mbc5(int rom_size_num, int ram_size_num) : Mbc(rom_size_num, ram_size_num) {
}

unsigned int Mbc5::rom_size() const {
    return (0x4000 * 512);
}

unsigned int Mbc5::ecart_size() const {
    return (16 * 0x2000);
}

byte Mbc5::read_byte(word address, UsageType usage) {
    if (address <= 0x3FFF) {
        rom_usage[address] = usage;
        return data[address];
    }

    else if (address <= 0x7FFF) {
        rom_usage  [rom_offset + (address - 0x4000)] = usage;
        return data[rom_offset + (address - 0x4000)];
    }

    else if (address >= 0xA000 && address <= 0xBFFF) {
        if (rom_type == 0x1A || rom_type == 0x1B || rom_type == 0x1D || rom_type == 0x1E) {
            if (ram_on)
                return ecart[ram_offset + (address - 0xA000)];
            else
                return 0xFF;
        }
    }

    else
        logger.log("Mbc5::read_byte can't access address 0x" + hex(address, 4), Logger::Warning);

    return 0;
}

void Mbc5::write_byte(word address, byte value) {
    if (address <= 0x1FFF) {
        if (rom_type == 0x1A || rom_type == 0x1B || rom_type == 0x1D || rom_type == 0x1E)
            ram_on = (value & 0x0F) == 0x0A;
    }

    else if (address <= 0x2FFF) {
        rom_bank = (rom_bank & 0xFF00) | value;
        rom_bank &= rom_bank_mask;
        rom_offset = rom_bank * 0x4000;
    }

    else if (address <= 0x3FFF) {
        value &= 0x01;
        rom_bank = (rom_bank & 0xFF) | (value << 8);
        rom_bank &= rom_bank_mask;
        rom_offset = rom_bank * 0x4000;
    }

    else if (address <= 0x5FFF) {
        ram_bank = value & 0x0F;
        ram_offset = 0x2000 * ram_bank;
    }

    else if (address >= 0xA000 && address <= 0xBFFF) {
        if (rom_type == 0x1A || rom_type == 0x1B || rom_type == 0x1D || rom_type == 0x1E) {
            if (ram_on)
                ecart[ram_offset + (address - 0xA000)] = value;
        }
    }

    else
        logger.log("Mbc5::write_byte can't access address 0x" + hex(address, 4), Logger::Warning);
}
