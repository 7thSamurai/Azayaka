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

#ifndef _MMU_HPP_
#define _MMU_HPP_

#include "component.hpp"
#include "types.hpp"

class State;
class GbcReg;

class Mmu : public Component
{
public:
    Mmu(GameBoy *gb);
    ~Mmu();

    byte read_byte(word address);
    void write_byte(word address, byte value);

    byte read_instr(word address); // Read an Instruction
    byte read_oper (word address); // Read an Operand

    void register_component(Component *component, word start_address, word end_address);

    byte get_interrupt_enable() const;
    void set_interrupt_enable(byte value);

    byte get_interrupt_flags() const;
    void set_interrupt_flags(byte value);

    byte get_key1() const;
    void set_key1(byte value);

    void save_state(State &state);
    void load_state(State &state);

    byte read(word address) override;
    void write(word address, byte value) override;

private:
    void reset();

    byte *wram[8];
    byte *hram;

    byte key1;
    byte wram_bank;
    byte interrupt_enable;
    byte interrupt_flags;

    Component *components[0x10000];
    GbcReg *gbc_reg;
};

#endif // _MMU_HPP_
