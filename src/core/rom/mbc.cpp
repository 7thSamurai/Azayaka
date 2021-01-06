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

#include "mbc.hpp"
#include "state.hpp"
#include "globals.hpp"
#include "defs.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"

#include <fstream>

Mbc::Mbc(int rom_size_num, int ram_size_num) : Cart() {
    rom_offset = 0x4000;
    ram_offset = 0;

    ram_on = 0;
    rom_bank = 0;
    ram_bank = 0;

    rom_bank_mask = (rom_size_num / 0x4000) - 1;
    ram_bank_mask = (ram_size_num / 0x2000) - 1;
}

bool Mbc::ram_enabled() const {
    return ram_on;
}

int Mbc::get_ram_bank() const {
    return ram_bank;
}

int Mbc::get_rom_bank() const {
    return rom_bank;
}

void Mbc::save_state(State &state) {
    state.write32(rom_offset);
    state.write32(ram_offset);

    state.write8(ram_on);
    state.write16(rom_bank);
    state.write8(ram_bank);

    state.write16(rom_bank_mask);
    state.write16(ram_bank_mask);
}

void Mbc::load_state(State &state) {
    rom_offset = state.read32();
    ram_offset = state.read32();

    ram_on   = state.read8();
    rom_bank = state.read16();
    ram_bank = state.read8();

    rom_bank_mask = state.read16();
    ram_bank_mask = state.read16();
}

int Mbc::get_usage(word address) {
    if (address <= 0x3FFF)
        return rom_usage[address];

    else // address <= 0x7FFF
        return rom_usage[rom_offset + (address - 0x4000)];
}
