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

#include "core/rom/mbc2.hpp"
#include "common/logger.hpp"
#include "common/string_utils.hpp"

Mbc2::Mbc2(int rom_size_num, int ram_size_num) : Mbc(rom_size_num, ram_size_num) {
}

unsigned int Mbc2::rom_size() const {
    return (0x4000 * 16);
}

unsigned int Mbc2::ecart_size() const {
    return 512;
}

byte Mbc2::read_byte(word address, UsageType usage) {
    if (address <= 0x3FFF) {
        rom_usage[address] = usage;
        return data[address];
    }

    else if (address <= 0x7FFF) {
        rom_usage  [rom_offset + (address - 0x4000)] = usage;
        return data[rom_offset + (address - 0x4000)];
    }

    else if (address >= 0xA000 && address <= 0xBFFF) {
        if (ram_on)
            return ecart[address & 0x01FF] | 0xF0;
        else
            return 0xFF;
    }

    else
        LOG_WARNING("Mbc2::read_byte can't access address 0x" + StringUtils::hex(address));

    return 0;
}

void Mbc2::write_byte(word address, byte value) {
    if (address <= 0x3FFF) {
        if (address & 0x100) {
            rom_bank = value & 0x0F;
            if (!rom_bank)
                rom_bank = 1;

            rom_bank &= rom_bank_mask;
            rom_offset = rom_bank * 0x4000;
        }

        else
            ram_on = (value & 0x0F) == 0x0A;
    }

    else if (address >= 0xA000 && address <= 0xBFFF) {
        if (ram_on)
            ecart[address & 0x01FF] = value & 0x0F;
    }

    else
        LOG_WARNING("Mbc2::write_byte can't access address 0x" + StringUtils::hex(address));
}
