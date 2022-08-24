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

#include "core/component.hpp"
#include "core/defs.hpp"
#include "core/types.hpp"

#include <string>

// Flags
#define ZF 0x80
#define NF 0x40
#define HF 0x20
#define CF 0x10

class State;

class Cpu : public Component
{
public:
    Cpu(GameBoy *gb);

    void init_gb_mode();
    void init_gbc_mode();

    void step();

    void trigger_interrupt(byte interrupt);

    void save_state(State &state);
    void load_state(State &state);

    int get_reg8(char r) const;
    int get_reg16(char r1, char r2) const;

    int set_reg8(char r, int value);
    int set_reg16(char r1, char r2, int value);

    int get_pc() const;
    int get_sp() const;

    bool is_blargg_done () const;
    bool is_mooneye_done() const;

private:
    void tick();
    void tick_double_speed();

    void tick4();
    void hdma_tick4();

    // Functions to read the registers when used as byte-pairs
    inline word af() const;
    inline word bc() const;
    inline word de() const;
    inline word hl() const;

    // Functions to write to the registers when used as byte-pairs
    inline void write_af(word af);
    inline void write_bc(word bc);
    inline void write_de(word de);
    inline void write_hl(word hl);

    inline void write_byte(word address, byte value);
    inline byte read_byte(word address);
    inline byte read_operand();
    inline byte read_instr();

    void skip_operand();

    inline void push(byte high, byte low);

    inline void set_flag  (byte flag);
    inline void clear_flag(byte flag);

    inline void update_flag(byte flag, bool state);

    void invalid_op();

    inline bool interrupts_to_do();
    void execute_interrupt();

    inline void inc_r(byte &r);
    inline void dec_r(byte &r);
    inline void add_a_n(byte n);
    inline void adc_a_n(byte n);
    inline void sub_a_n(byte n);
    inline void sbc_a_n(byte n);
    inline void and_a_n(byte n);
    inline void or_a_n(byte n);
    inline void xor_a_n(byte n);
    inline void cp_a_n(byte n);

    inline void rlc_r(byte &r);
    inline void rrc_r(byte &r);
    inline void rl_r(byte &r);
    inline void rr_r(byte &r);
    inline void sla_r(byte &r);
    inline void sra_r(byte &r);
    inline void swap_r(byte &r);
    inline void srl_r(byte &r);

    inline void bit_n(byte n, byte bit);
    inline void res_r(byte &r, byte bit);
    inline void set_r(byte &r, byte bit);

    inline void res_hlp(byte bit);
    inline void set_hlp(byte bit);

    inline void rst_n(byte n);

    inline void jr_n();
    inline void jp_nn();
    inline void call_nn();
    inline void ret();

    inline void push_rr(byte high, byte low);
    inline void pop_rr(byte &high, byte &low);

    inline void ld_rr_nn(byte &high, byte &low);
    inline void inc_rr(byte &high, byte &low);
    inline void dec_rr(byte &high, byte &low);
    inline void add_hl_rr(byte high, byte low);

    inline word add_sp_n();

    void stop();

    void run_instr(byte instr);

    bool IME; // Interrupt Master Enable Flag

    byte a, b, c, d, e, f, h, l; // 8-bit Registers
    word pc, sp; // 16-bit Registers

    bool double_speed;

    enum Mode {
        Mode_Normal,
        Mode_Halt,
        Mode_HaltBug,
        Mode_HaltDI,
        Mode_Stop,
        Mode_EnableIME
    };

    Mode mode;
};
