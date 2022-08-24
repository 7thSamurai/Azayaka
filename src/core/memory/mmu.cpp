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

#include "core/memory/mmu.hpp"
#include "core/memory/gbc_reg.hpp"
#include "core/gameboy.hpp"
#include "core/state.hpp"
#include "core/defs.hpp"
#include "common/logger.hpp"
#include "common/string_utils.hpp"

Mmu::Mmu(GameBoy *gb) : Component(gb) {
    for (int i = 0; i < 8; i++)
        wram[i] = new byte[0x1000];

    hram = new byte[0x0080];

    reset();

    register_component(this, 0x0000, 0xFFFF);

    gbc_reg = new GbcReg(gb);
    register_component(gbc_reg, 0xFF6C, 0xFF6C);
    register_component(gbc_reg, 0xFF72, 0xFF77);
}

Mmu::~Mmu() {
    for (int i = 0; i < 8; i++)
        delete[] wram[i];

    delete[] hram;
    delete gbc_reg;
}

byte Mmu::read_byte(word address) {
    return components[address]->read(address);
}

void Mmu::write_byte(word address, byte value) {
    components[address]->write(address, value);
}

byte Mmu::read_instr(word address) {
    return components[address]->read_instruction(address);
}

byte Mmu::read_oper(word address) {
    return components[address]->read_operand(address);
}

void Mmu::register_component(Component *component, word start_address, word end_address) {
    for (int i = start_address; i <= end_address; i++)
        components[i] = component;
}

byte Mmu::get_interrupt_enable() const {
    return interrupt_enable;
}

void Mmu::set_interrupt_enable(byte value) {
    interrupt_enable = value;
}

byte Mmu::get_interrupt_flags() const {
    return interrupt_flags;
}

void Mmu::set_interrupt_flags(byte value) {
    interrupt_flags = value;
}

byte Mmu::get_key1() const {
    return key1;
}

void Mmu::set_key1(byte value) {
    key1 = value;
}

// Function to reset the MMU
void Mmu::reset() {
    for (int i = 0; i < 8; i++)
        std::fill(wram[i], wram[i]+0x1000, 0);

    std::fill(hram, hram+0x80, 0);

    key1 = 0;

    interrupt_enable = 0;
    interrupt_flags  = 0;

    wram_bank = 0;
}

void Mmu::save_state(State &state) {
    gbc_reg->save_state(state);

    state.write8(key1);
    state.write8(wram_bank);
    state.write8(interrupt_enable);
    state.write8(interrupt_flags);

    state.write_data(hram, 0x0080);

    if (gb->gbc_mode) {
        for (int i = 0; i < 8; i++)
            state.write_data(wram[i], 0x1000);
    }

    else {
        state.write_data(wram[0], 0x1000);
        state.write_data(wram[1], 0x1000);
    }
}

void Mmu::load_state(State &state) {
    gbc_reg->load_state(state);

    key1             = state.read8();
    wram_bank        = state.read8();
    interrupt_enable = state.read8();
    interrupt_flags  = state.read8();

    state.read_data(hram, 0x0080);

    if (gb->gbc_mode) {
        for (int i = 0; i < 8; i++)
            state.read_data(wram[i], 0x1000);
    }

    else {
        state.read_data(wram[0], 0x1000);
        state.read_data(wram[1], 0x1000);
    }
}

byte Mmu::read(word address) {
    if (address >= 0xC000 && address <= 0xCFFF)
        return wram[0][address - 0xC000];

    else if (address >= 0xD000 && address <= 0xDFFF) {
        if (gb->gbc_mode) {
            int bank = wram_bank != 0 ? wram_bank : 1;

            return wram[bank][address - 0xD000];
        }
        else
            return wram[1][address - 0xD000];
    }

    else if (address >= 0xE000 && address <= 0xEFFF)
        return wram[0][address - 0xE000];

    else if (address >= 0xF000 && address <= 0xFDFF)
        return wram[1][address - 0xF000]; // FIXME: Is this how it works(GBC)?

    else if (address >= 0xFEA0 && address <= 0xFEFF)
        return 0; // Unusable

    else if (address == 0xFF0F)
        return interrupt_flags | 0xE0; // Bits 5-7 return 0

    else if (address == 0xFF4D)
        return gb->gbc_mode ? (key1 | 0x7E) : 0xFF; // Bits 1-6 return 1

    else if (address == 0xFF70)
        return gb->gbc_mode ? (wram_bank | 0xF8): 0xFF;

    else if (address >= 0xFF80 && address <= 0xFFFE)
        return hram[address - 0xFF80];

    else if (address == 0xFFFF)
        return interrupt_enable;

    // Unmapped IO registers
    switch (address) {
        case 0xFF03:
        case 0xFF08 ... 0xFF0E:
        case 0xFF4C:
        case 0xFF4E:
        case 0xFF56: // TODO: RP
        case 0xFF57 ... 0xFF67:
        case 0xFF6D ... 0xFF6F:
        case 0xFF71:
        case 0xFF78 ... 0xFF7F:
            return 0xFF;
    }

    LOG_WARNING("Mmu::read can't access address 0x" + hex(address));

    return 0;
}

void Mmu::write(word address, byte value) {
    if (address >= 0xC000 && address <= 0xCFFF)
        wram[0][address - 0xC000] = value;

    else if (address >= 0xD000 && address <= 0xDFFF) {
        if (gb->gbc_mode) {
            int bank = wram_bank != 0 ? wram_bank : 1;

            wram[bank][address - 0xD000] = value;
        }
        else
            wram[1][address - 0xD000] = value;
    }

    else if (address >= 0xE000 && address <= 0xEFFF)
        wram[0][address - 0xE000] = value;

    else if (address >= 0xF000 && address <= 0xFDFF)
        wram[1][address - 0xF000] = value; // FIXME: Is this how it works(GBC)?

    else if (address >= 0xFEA0 && address <= 0xFEFF); // Unusable

    else if (address == 0xFF0F)
        interrupt_flags = value & 0x1F; // Only the lower 5 bits are writable

    else if (address == 0xFF4D)
        key1 |= value & BIT0;

    else if (address == 0xFF70) {
        if (gb->gbc_mode)
            wram_bank = value & 0b111; // Only the lower 3 bits are writable
    }

    else if (address >= 0xFF80 && address <= 0xFFFE)
        hram[address - 0xFF80] = value;

    else if (address == 0xFFFF)
        interrupt_enable = value;

    else {
        // Unmapped IO registers
        switch (address) {
            case 0xFF03:
            case 0xFF08 ... 0xFF0E:
            case 0xFF4C:
            case 0xFF4E:
            case 0xFF56: // TODO: RP
            case 0xFF57 ... 0xFF67:
            case 0xFF6D ... 0xFF6F:
            case 0xFF71:
            case 0xFF78 ... 0xFF7F:
                return;
        }

        LOG_WARNING("Mmu::write can't access address 0x" + hex(address));
    }
}
