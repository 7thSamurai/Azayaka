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

#include "mbc1.hpp"
#include "globals.hpp"
#include "state.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"

Mbc1::Mbc1(int rom_size_num, int ram_size_num) : Mbc(rom_size_num, ram_size_num) {
    mode = 0;

    hi_bank  = 0;
    rom_bank = 1;
}

unsigned int Mbc1::rom_size() const {
    return (0x4000 * 128);
}

unsigned int Mbc1::ecart_size() const {
    return (0x2000 * 4);
}

byte Mbc1::read_byte(word address, UsageType usage) {
    if (address <= 0x3FFF) {
        if (mode) {
            int offset = ((hi_bank << get_hi_shift()) & rom_bank_mask) * 0x4000;

            rom_usage  [offset + address] = usage;
            return data[offset + address];
        }
        else {
            rom_usage[address] = usage;
            return data[address];
        }
    }

    else if (address <= 0x7FFF) {
        rom_usage  [rom_offset + (address - 0x4000)] = usage;
        return data[rom_offset + (address - 0x4000)];
    }

    else if (address >= 0xA000 && address <= 0xBFFF) {
        if (ram_on)
            return ecart[ram_offset + (address - 0xA000)];
        else
            return 0xFF;
    }

    else
        logger.log("Mbc1::read_byte can't access address 0x" + hex(address, 4), Logger::Warning);

    return 0;
}

void Mbc1::write_byte(word address, byte value) {
    if (address <= 0x1FFF) {
        if (rom_type == 2 || rom_type == 3)
            ram_on = (value & 0x0F) == 0x0A;
    }

    else if (address <= 0x3FFF) {
        value &= 0x1F;

        if (!value) value = 1;
        rom_bank = value & rom_bank_mask;

        if (is_multicart)
            rom_bank &= 0x0F;

        update_rom_ram();
    }

    else if (address <= 0x5FFF) {
        hi_bank = value & 0b11;

        update_rom_ram();
    }

    else if (address <= 0x7FFF) {
        mode = (value & 1) != 0;

        update_rom_ram();
    }

    else if (address >= 0xA000 && address <= 0xBFFF) {
        if (ram_on)
            ecart[ram_offset + (address - 0xA000)] = value;
    }

    else
        logger.log("Mbc1::write_byte can't access address 0x" + hex(address, 4), Logger::Warning);
}

void Mbc1::check_multicart() {
    int count = 0;

    for (int bank = 0; bank < 4; bank++) {
        bool compare = 1;

        for (int addr = 0x0104; addr <= 0x0133; addr++) {
            if (data[bank*0x40000 + addr] != logo[addr-0x0104]) {
                compare = 0;
                break;
            }
        }

        count += compare ? 1 : 0;
    }

    is_multicart = count > 1;
}

bool Mbc1::get_mode() const {
    return mode;
}

void Mbc1::save_state(State &state) {
    Mbc::save_state(state);

    state.write8(mode);
    state.write8(hi_bank);
    state.write8(is_multicart);
}

void Mbc1::load_state(State &state) {
    Mbc::load_state(state);

    mode    = state.read8();
    hi_bank = state.read8();
    is_multicart = state.read8();
}

void Mbc1::update_rom_ram() {
    if (mode) { // RAM Mode
        ram_bank = hi_bank & ram_bank_mask;
        ram_offset = ram_bank * 0x2000;
    }

    else // ROM Mode
        ram_offset = 0;

    rom_offset = ((rom_bank | (hi_bank << get_hi_shift())) & rom_bank_mask) * 0x4000;
}

byte Mbc1::get_hi_shift() const {
    return is_multicart ? 4 : 5;
}
