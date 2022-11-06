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

#include "core/cpu/cpu.hpp"
#include "core/gameboy.hpp"
#include "core/cpu/timer.hpp"
#include "core/memory/mmu.hpp"
#include "core/memory/dma.hpp"
#include "core/memory/hdma.hpp"
#include "core/gpu/gpu.hpp"
#include "core/audio/apu.hpp"
#include "core/serial/serial.hpp"
#include "core/state.hpp"
#include "common/logger.hpp"
#include "common/string_utils.hpp"

#include <fstream>

Cpu::Cpu(GameBoy *gb) : Component(gb) {
    a = b = c = d = e = f = h = l = 0x00;

    pc = sp = 0x0000;

    IME = 0;
    double_speed = 0;

    mode = Mode_Normal;
}

// Function to set the start-up values
// for the registers in GB mode
void Cpu::init_gb_mode() {
    // Initial 8-bit registers values
    a = 0x01;
    b = 0x00;
    c = 0x13;
    d = 0x00;
    e = 0xD8;
    f = 0xB0;
    h = 0x01;
    l = 0x4D;

    // Initial 16-bit registers values
    pc = 0x0100;
    sp = 0xFFFE;
}

// Function to set the start-up values
// for the registers in GBC mode
void Cpu::init_gbc_mode() {
    // Initial 8-bit registers values
    a = 0x11;
    b = 0x00;
    c = 0x00;
    d = 0xFF;
    e = 0x56;
    f = 0x80;
    h = 0x00;
    l = 0x0D;

    // Initial 16-bit registers values
    pc = 0x0100;
    sp = 0xFFFE;
}

// Function to run one CPU frame
void Cpu::step() {
    if (gb->hdma->is_copying()) {
        hdma_tick4();

        return;
    }

    bool interrupt = 0;

    switch (mode) {
        case Mode_Normal:
            run_instr(read_instr());
            interrupt = IME && interrupts_to_do();
            break;

        case Mode_Halt:
        case Mode_Stop:
            tick4();
            interrupt = interrupts_to_do();
            break;

        case Mode_HaltBug:
            {
                byte instr = read_instr();
                pc--;
                run_instr(instr);

                mode = Mode_Normal;
                interrupt = IME && interrupts_to_do();
            }
            break;

        case Mode_HaltDI:
            tick4();

            if (interrupts_to_do())
                mode = Mode_Normal;
            break;

        case Mode_EnableIME:
            IME = 1;
            mode = Mode_Normal;

            run_instr(read_instr());
            interrupt = IME && interrupts_to_do();
            break;
    }

    if (interrupt)
        execute_interrupt();
}

void Cpu::tick() {
    gb->dma   ->tick();
    gb->timer ->tick();
    gb->serial->tick();
    gb->gpu   ->tick();
    gb->apu   ->tick();
}

void Cpu::tick_double_speed() {
    gb->dma   ->tick();
    gb->timer ->tick();
    gb->serial->tick();
}

void Cpu::tick4() {
    if (double_speed) {
        tick();
        tick_double_speed();
        tick();
        tick_double_speed();
    }
    else {
        tick();
        tick();
        tick();
        tick();
    }
}

void Cpu::hdma_tick4() {
    if (double_speed) {
        tick();
        tick_double_speed();
        tick();
        tick_double_speed();

        // HDMA takes twice as long in double-speed mode
        gb->hdma->tick();
    }
    else {
        tick();
        tick();
        tick();
        tick();

        gb->hdma->tick();
        gb->hdma->tick();
    }
}

void Cpu::trigger_interrupt(byte interrupt) {
    byte IF = gb->mmu->get_interrupt_flags();

    switch (interrupt) {
        case INT40: IF |= BIT0; break;
        case INT48: IF |= BIT1; break;
        case INT50: IF |= BIT2; break;
        case INT58: IF |= BIT3; break;
        case INT60: IF |= BIT4; break;

        default:
            break;
    }

    gb->mmu->set_interrupt_flags(IF);
}

void Cpu::save_state(State &state) {
    state.write8(IME);
    state.write8(a);
    state.write8(b);
    state.write8(c);
    state.write8(d);
    state.write8(e);
    state.write8(f);
    state.write8(h);
    state.write8(l);
    state.write16(pc);
    state.write16(sp);
    state.write8(double_speed);
    state.write8(mode);
}

void Cpu::load_state(State &state) {
    IME          = state.read8();
    a            = state.read8();
    b            = state.read8();
    c            = state.read8();
    d            = state.read8();
    e            = state.read8();
    f            = state.read8();
    h            = state.read8();
    l            = state.read8();
    pc           = state.read16();
    sp           = state.read16();
    double_speed = state.read8();
    mode         = (Mode)state.read8();
}

int Cpu::get_reg8(char r) const {
    switch (r) {
        case 'a': return a;
        case 'b': return b;
        case 'c': return c;
        case 'd': return d;
        case 'e': return e;
        case 'f': return f;
        case 'h': return h;
        case 'l': return l;

        default: break;
    }

    return -1;
}

int Cpu::get_reg16(char r1, char r2) const {
    if      (r1 == 'a' && r2 == 'f') return af();
    else if (r1 == 'b' && r2 == 'c') return bc();
    else if (r1 == 'd' && r2 == 'e') return de();
    else if (r1 == 'h' && r2 == 'l') return hl();
    else if (r1 == 'p' && r2 == 'c') return pc;
    else if (r1 == 's' && r2 == 'p') return sp;

    return -1;
}

int Cpu::set_reg8(char r, int value) {
    switch (r) {
        case 'a': a = value; return 0;
        case 'b': b = value; return 0;
        case 'c': c = value; return 0;
        case 'd': d = value; return 0;
        case 'e': e = value; return 0;
        case 'f': f = value; return 0;
        case 'h': h = value; return 0;
        case 'l': l = value; return 0;

        default: break;
    }

    return -1;
}

int Cpu::set_reg16(char r1, char r2, int value) {
    if      (r1 == 'a' && r2 == 'f') { write_af(value); return 0; }
    else if (r1 == 'b' && r2 == 'c') { write_bc(value); return 0; }
    else if (r1 == 'd' && r2 == 'e') { write_de(value); return 0; }
    else if (r1 == 'h' && r2 == 'l') { write_hl(value); return 0; }
    else if (r1 == 'p' && r2 == 'c') { pc = value;      return 0; }
    else if (r1 == 's' && r2 == 'p') { sp = value;      return 0; }

    return -1;
}

int Cpu::get_pc() const {
    return pc;
}

int Cpu::get_sp() const {
    return sp;
}

bool Cpu::is_blargg_done() const {
    if (gb->mmu->read_byte(pc+0) == 0x18 && // jp
        gb->mmu->read_byte(pc+1) == 0xFE)   // -2
        return 1;

    else if (gb->mmu->read_byte(pc+0) == 0xc3 &&        // jp
             gb->mmu->read_byte(pc+1) == (pc & 0xFF) && // LSB of pc
             gb->mmu->read_byte(pc+2) == (pc >> 8))     // MSB of pc
             return 1;

    return 0;
}

bool Cpu::is_mooneye_done() const {
    // See https://github.com/Gekkio/mooneye-test-suite/#passfail-reporting
    if (gb->mmu->read_byte(pc+0) == 0x40 && // ld b, b
        gb->mmu->read_byte(pc+1) == 0x18 && // jr
        gb->mmu->read_byte(pc+2) == 0xFE)   // -2
        return 1;

    return 0;
}

// Functions to read the registers when used as byte-pairs
inline word Cpu::af() const {
    return a << 8 | f;
}

inline word Cpu::bc() const {
    return b << 8 | c;
}

inline word Cpu::de() const {
    return d << 8 | e;
}

inline word Cpu::hl() const {
    return h << 8 | l;
}

// Functions to write to the registers when used as byte-pairs
inline void Cpu::write_af(word af) {
    a = af >> 8;
    f = af & 0xF0; // Lower 4 bits should always be zero
}

inline void Cpu::write_bc(word bc) {
    b = bc >> 8;
    c = bc & 0xFF;
}

inline void Cpu::write_de(word de) {
    d = de >> 8;
    e = de & 0xFF;
}

inline void Cpu::write_hl(word hl) {
    h = hl >> 8;
    l = hl & 0xFF;
}

inline void Cpu::write_byte(word address, byte value) {
    tick4();
    gb->mmu->write_byte(address, value);
}

inline byte Cpu::read_byte(word address) {
    tick4();
    return gb->mmu->read_byte(address);
}

inline byte Cpu::read_operand() {
    tick4();
    return gb->mmu->read_oper(pc++);
}

inline byte Cpu::read_instr() {
    tick4();
    byte instr = gb->mmu->read_instr(pc++);

    return instr;
}

void Cpu::skip_operand() {
    tick4();
    gb->mmu->read_oper(pc++);
}

inline void Cpu::push(byte high, byte low) {
    write_byte(--sp, high);
    write_byte(--sp, low);
}

inline void Cpu::set_flag(byte flag) {
    f |= flag;
}

inline void Cpu::clear_flag(byte flag) {
    f &= ~flag;
}

inline void Cpu::update_flag(byte flag, bool state) {
    if (state)
        f |= flag;
    else
        f &= ~flag;
}

void Cpu::invalid_op() {
    LOG_ERROR("Invalid Instruction 0x" + StringUtils::hex(gb->mmu->read_byte(pc-1)) + " at address: 0x" + StringUtils::hex<u16>(pc-1));
}

inline bool Cpu::interrupts_to_do() {
    return (gb->mmu->get_interrupt_enable() & gb->mmu->get_interrupt_flags()) & 0x1F;
}

void Cpu::execute_interrupt() {
    if (IME) {
        write_byte(--sp, pc >> 8); // Save the high-byte of PC

        byte IE = gb->mmu->get_interrupt_enable();
        byte IF = gb->mmu->get_interrupt_flags();
        byte interrupt = IE & IF;

        // This is zero that way the interrupt can be canceled
        word vector = 0;

        // Check Interrupts in order of highest priority
        if      (interrupt & BIT0) { IF &= ~BIT0; vector = 0x40; } // V-Blank
        else if (interrupt & BIT1) { IF &= ~BIT1; vector = 0x48; } // LCD STAT
        else if (interrupt & BIT2) { IF &= ~BIT2; vector = 0x50; } // Timer
        else if (interrupt & BIT3) { IF &= ~BIT3; vector = 0x58; } // Serial
        else if (interrupt & BIT4) { IF &= ~BIT4; vector = 0x60; } // Joypad

        gb->mmu->set_interrupt_flags(IF);

        write_byte(--sp, pc & 0xFF); // Save the low-byte of PC
        pc = vector;

        IME = false;

        tick4();
        tick4();
        tick4();
    }

    mode = Mode_Normal;
}

inline void Cpu::inc_r(byte &r) {
    clear_flag(NF);
    update_flag(HF, (r & 0x0F) == 0x0F);

    r++;

    update_flag(ZF, r == 0);
}

inline void Cpu::dec_r(byte &r) {
    set_flag(NF);
    update_flag(HF, (r & 0x0F) == 0x00);

    r--;

    update_flag(ZF, r == 0);
}

inline void Cpu::add_a_n(byte n) {
    word result = a + n;

    clear_flag(NF);

    update_flag(CF, result & 0xFF00);
    update_flag(HF, ((a & 0x0F) + (n & 0x0F)) > 0x0F);

    a = result & 0xFF;

    update_flag(ZF, a == 0);
}

inline void Cpu::adc_a_n(byte n) {
    byte carry = (f & CF) ? 1 : 0;
    word result = a + n + carry;

    clear_flag(NF);

    update_flag(CF, result & 0xFF00);
    update_flag(HF, ((a & 0x0F) + (n & 0x0F) + carry) > 0x0F);

    a = result & 0xFF;

    update_flag(ZF, a == 0);
}

inline void Cpu::sub_a_n(byte n) {
    int result = (a - n) & 0xFF;

    set_flag(NF);

    update_flag(ZF, result == 0);
    update_flag(CF, a < n);
    update_flag(HF, (a & 0x0F) < (n & 0x0F));

    a = result;
}

inline void Cpu::sbc_a_n(byte n) {
    byte carry = (f & CF) ? 1 : 0;
    int result = a - n - carry;

    set_flag(NF);

    update_flag(CF, result < 0);

    result &= 0xFF;

    update_flag(ZF, result == 0);
    update_flag(HF, ((a ^ n ^ result) & BIT4) != 0);

    a = result;
}

inline void Cpu::and_a_n(byte n) {
    a &= n;

    clear_flag(CF | NF);
    set_flag(HF);
    update_flag(ZF, a == 0);
}

inline void Cpu::or_a_n(byte n) {
    a |= n;

    clear_flag(NF | HF | CF);
    update_flag(ZF, a == 0);
}

inline void Cpu::xor_a_n(byte n) {
    a ^= n;

    clear_flag(NF | HF | CF);
    update_flag(ZF, a == 0);
}

inline void Cpu::cp_a_n(byte n) {
    int result = (a - n) & 0xFF;

    set_flag(NF);

    update_flag(ZF, result == 0);
    update_flag(CF, a < n);
    update_flag(HF, (a & 0x0F) < (n & 0x0F));
}

inline void Cpu::rlc_r(byte &r) {
    clear_flag(NF | HF);

    byte result = r << 1; // Rotate bits left

    if (r & BIT7) {
        set_flag(CF); // If the seventh bit(far left) is set, then set Carry-Flag
        result |= BIT0;
    }
    else
        clear_flag(CF);

    update_flag(ZF, result == 0);

    r = result;
}

inline void Cpu::rrc_r(byte &r) {
    clear_flag(NF | HF);

    byte result = r >> 1; // Rotate bits right

    if (r & BIT0) {
        set_flag(CF); // If first bit(far right) is set, then set Carry-Flag
        result |= BIT7;
    }
    else
        clear_flag(CF);

    update_flag(ZF, result == 0);

    r = result;
}

inline void Cpu::rl_r(byte &r) {
    byte result = r << 1; // Rotate bits left
    result |= (f & CF) ? BIT0 : 0;

    clear_flag(NF | HF);

    update_flag(CF, r & BIT7);
    update_flag(ZF, result == 0);

    r = result;
}

inline void Cpu::rr_r(byte &r) {
    // Rotate A right through the Carry-Flag
    byte result = r >> 1;
    result |= (f & CF) ? BIT7 : 0;

    clear_flag(NF | HF);

    update_flag(CF, r & BIT0);
    update_flag(ZF, result == 0);

    r = result;
}

inline void Cpu::sla_r(byte &r) {
    update_flag(CF, r & BIT7);

    r <<= 1;

    clear_flag(NF | HF);
    update_flag(ZF, r == 0);
}

inline void Cpu::sra_r(byte &r) {
    update_flag(CF, r & BIT0);

    r = (r >> 1) | (r & BIT7);

    clear_flag(NF | HF);
    update_flag(ZF, r == 0);
}

inline void Cpu::swap_r(byte &r) {
    r = r << 4 | r >> 4;

    clear_flag(NF | HF | CF);
    update_flag(ZF, r == 0);
}

inline void Cpu::srl_r(byte &r) {
    update_flag(CF, r & BIT0);

    r >>= 1;

    clear_flag(NF | HF);
    update_flag(ZF, r == 0);
}

inline void Cpu::bit_n(byte n, byte bit) {
    clear_flag(NF);
    set_flag(HF);

    update_flag(ZF, (n & (1 << bit)) == 0);
}

inline void Cpu::res_r(byte &r, byte bit) {
    r &= ~(1 << bit);
}

inline void Cpu::set_r(byte &r, byte bit) {
    r |= (1 << bit);
}

inline void Cpu::res_hlp(byte bit) {
    word addr = hl();

    write_byte(addr, read_byte(addr) & (~(1 << bit)));
}

inline void Cpu::set_hlp(byte bit) {
    word addr = hl();

    write_byte(addr, read_byte(addr) | (1 << bit));
}

inline void Cpu::rst_n(byte n) {
    push_rr(pc >> 8, pc & 0xFF);
    pc = n;
}

inline void Cpu::jr_n() {
    pc += s8(read_operand());

    tick4();
}

inline void Cpu::jp_nn() {
    byte low  = read_operand();
    byte high = read_operand();

    pc = high << 8 | low;

    tick4();
}

inline void Cpu::call_nn() {
    word PC = pc + 2;

    jp_nn();
    push(PC >> 8, PC & 0xFF);
}

inline void Cpu::ret() {
    byte high, low;
    pop_rr(high, low);

    pc = high << 8 | low;

    tick4();
}

inline void Cpu::push_rr(byte high, byte low) {
    tick4();

    push(high, low);
}

inline void Cpu::pop_rr(byte &high, byte &low) {
    low  = read_byte(sp++);
    high = read_byte(sp++);
}

inline void Cpu::ld_rr_nn(byte &high, byte &low) {
    low  = read_operand();
    high = read_operand();
}

inline void Cpu::inc_rr(byte &high, byte &low) {
    word value = high << 8 | low;
    value++;

    high = value >> 8;
    low  = value & 0xFF;

    tick4();
}

inline void Cpu::dec_rr(byte &high, byte &low) {
    word value = high << 8 | low;
    value--;

    high = value >> 8;
    low  = value & 0xFF;

    tick4();
}

inline void Cpu::add_hl_rr(byte high, byte low) {
    word num1 = h << 8 | l;
    word num2 = high << 8 | low;

    unsigned int result = num1 + num2;

    clear_flag(NF);

    update_flag(CF, result > 0xFFFF);
    update_flag(HF, ((num1 & 0x0FFF) + (num2 & 0x0FFF)) > 0x0FFF);

    h = result >> 8;
    l = result & 0xFF;

    tick4();
}

inline word Cpu::add_sp_n() {
    word result = sp + s8(read_operand());

    clear_flag(ZF | NF);

    update_flag(HF, (result & 0x0F) < (sp & 0x0F));
    update_flag(CF, (result & 0xFF) < (sp & 0xFF));

    tick4();

    return result;
}

// TODO: Verify that this is correct
void Cpu::stop() {
    if (gb->gbc_mode) {
        byte key1 = gb->mmu->get_key1();

        if (key1 & BIT0) {
            double_speed = !double_speed;

            gb->mmu->set_key1((key1 & BIT7) ? 0 : BIT7);
        }
    }

    else
        mode = Mode_Stop;
}

void Cpu::run_instr(byte instr) {
    switch (instr) {
        case 0x00:
            break;
        case 0x01:
            ld_rr_nn(b, c);
            break;
        case 0x02:
            write_byte(bc(), a);
            break;
        case 0x03:
            inc_rr(b, c);
            break;
        case 0x04:
            inc_r(b);
            break;
        case 0x05:
            dec_r(b);
            break;
        case 0x06:
            b = read_operand();
            break;
        case 0x07:
            rlc_r(a);
            clear_flag(ZF);
            break;
        case 0x08:
            {
                byte low  = read_operand();
                byte high = read_operand();

                word addr = high << 8 | low;

                write_byte(addr, sp & 0xFF);
                write_byte(addr+1, sp >> 8);
            }
            break;

        case 0x09:
            add_hl_rr(b, c);
            break;
        case 0x0a:
            a = read_byte(bc());
            break;
        case 0x0b:
            dec_rr(b, c);
            break;
        case 0x0c:
            inc_r(c);
            break;
        case 0x0d:
            dec_r(c);
            break;
        case 0x0e:
            c = read_operand();
            break;
        case 0x0f:
            rrc_r(a);
            clear_flag(ZF);
            break;
        case 0x10: stop(); break;
        case 0x11:
            ld_rr_nn(d, e);
            break;
        case 0x12:
            write_byte(de(), a);
            break;
        case 0x13:
            inc_rr(d, e);
            break;
        case 0x14:
            inc_r(d);
            break;
        case 0x15:
            dec_r(d);
            break;
        case 0x16:
            d = read_operand();
            break;
        case 0x17:
            rl_r(a);
            clear_flag(ZF);
            break;
        case 0x18:
            jr_n();
            break;
        case 0x19:
            add_hl_rr(d, e);
            break;
        case 0x1a:
            a = read_byte(de());
            break;
        case 0x1b:
            dec_rr(d, e);
            break;
        case 0x1c:
            inc_r(e);
            break;
        case 0x1d:
            dec_r(e);
            break;
        case 0x1e:
            e = read_operand();
            break;
        case 0x1f:
            rr_r(a);
            clear_flag(ZF);
            break;
        case 0x20:
            if (f & ZF)
                skip_operand();
            else
                jr_n();
            break;
        case 0x21:
            ld_rr_nn(h, l);
            break;
        case 0x22:
            {
                word addr = hl();
                write_byte(addr, a);

                write_hl(addr+1);
            }
            break;
        case 0x23:
            inc_rr(h, l);
            break;
        case 0x24:
            inc_r(h);
            break;
        case 0x25:
            dec_r(h);
            break;
        case 0x26:
            h = read_operand();
            break;
        case 0x27:
            {
                byte cf = f & CF;
                byte hf = f & HF;
                byte nf = f & NF;

                if (!nf) {
                    if (cf || a > 0x99) {
                        a += 0x60;
                        set_flag(CF);
                    }
                    if (hf || (a & 0x0F) > 0x09)
                        a += 0x6;
                }
                else {
                    if (cf)
                        a -= 0x60;
                    if (hf)
                        a -= 0x6;
                }

                clear_flag(HF);
                update_flag(ZF, a == 0);
            }
            break;
        case 0x28:
            if (f & ZF)
                jr_n();
            else
                skip_operand();
            break;
        case 0x29:
            add_hl_rr(h, l);
            break;
        case 0x2a:
            {
                word addr = hl();
                a = read_byte(addr);

                write_hl(addr+1);
            }
            break;
        case 0x2b:
            dec_rr(h, l);
            break;
        case 0x2c:
            inc_r(l);
            break;
        case 0x2d:
            dec_r(l);
            break;
        case 0x2e:
            l = read_operand();
            break;
        case 0x2f:
            set_flag(NF | HF);
            a ^= 0xFF;
            break;
        case 0x30:
            if (f & CF)
                skip_operand();
            else
                jr_n();
            break;
        case 0x31:
            {
                byte low  = read_operand();
                byte high = read_operand();

                sp = high << 8 | low;
            }
            break;
        case 0x32:
            {
                word addr = hl();
                write_byte(addr, a);

                write_hl(addr-1);
            }
            break;
        case 0x33:
            sp++;
            tick4();
            break;
        case 0x34:
            {
                word addr  = hl();
                byte value = read_byte(addr);

                inc_r(value);
                write_byte(addr, value);
            }
            break;
        case 0x35:
            {
                word addr  = hl();
                byte value = read_byte(addr);

                dec_r(value);
                write_byte(addr, value);
            }
            break;
        case 0x36:
            write_byte(hl(), read_operand());
            break;
        case 0x37:
            clear_flag(NF | HF);
            set_flag(CF);
            break;
        case 0x38:
            if (f & CF)
                jr_n();
            else
                skip_operand();
            break;
        case 0x39:
            add_hl_rr(sp >> 8, sp & 0xFF);
            break;
        case 0x3a:
            {
                word addr = hl();
                a = read_byte(addr);

                write_hl(addr-1);
            }
            break;
        case 0x3b:
            sp--;
            tick4();
            break;
        case 0x3c:
            inc_r(a);
            break;
        case 0x3d:
            dec_r(a);
            break;
        case 0x3e:
            a = read_operand();
            break;
        case 0x3f:
            clear_flag(NF | HF);

            if (f & CF) clear_flag(CF);
            else set_flag(CF);
            break;

        case 0x40: break; // LD B, B
        case 0x41: b = c; break;
        case 0x42: b = d; break;
        case 0x43: b = e; break;
        case 0x44: b = h; break;
        case 0x45: b = l; break;
        case 0x46: b = read_byte(hl()); break;
        case 0x47: b = a; break;

        case 0x48: c = b; break;
        case 0x49: break; // LD C, C
        case 0x4a: c = d; break;
        case 0x4b: c = e; break;
        case 0x4c: c = h; break;
        case 0x4d: c = l; break;
        case 0x4e: c = read_byte(hl()); break;
        case 0x4f: c = a; break;

        case 0x50: d = b; break;
        case 0x51: d = c; break;
        case 0x52: break; // LD D, D
        case 0x53: d = e; break;
        case 0x54: d = h; break;
        case 0x55: d = l; break;
        case 0x56: d = read_byte(hl()); break;
        case 0x57: d = a; break;

        case 0x58: e = b; break;
        case 0x59: e = c; break;
        case 0x5a: e = d; break;
        case 0x5b: break; // LD E, E
        case 0x5c: e = h; break;
        case 0x5d: e = l; break;
        case 0x5e: e = read_byte(hl()); break;
        case 0x5f: e = a; break;

        case 0x60: h = b; break;
        case 0x61: h = c; break;
        case 0x62: h = d; break;
        case 0x63: h = e; break;
        case 0x64: break; // LD H, H
        case 0x65: h = l; break;
        case 0x66: h = read_byte(hl()); break;
        case 0x67: h = a; break;

        case 0x68: l = b; break;
        case 0x69: l = c; break;
        case 0x6a: l = d; break;
        case 0x6b: l = e; break;
        case 0x6c: l = h; break;
        case 0x6d: break; // LD L, L
        case 0x6e: l = read_byte(hl()); break;
        case 0x6f: l = a; break;

        case 0x70: write_byte(hl(), b); break;
        case 0x71: write_byte(hl(), c); break;
        case 0x72: write_byte(hl(), d); break;
        case 0x73: write_byte(hl(), e); break;
        case 0x74: write_byte(hl(), h); break;
        case 0x75: write_byte(hl(), l); break;

        case 0x76:
            if (IME)
                mode = Mode_Halt;
            else {
                if (gb->mmu->get_interrupt_enable() & gb->mmu->get_interrupt_flags() & 0x1F)
                    mode = Mode_HaltBug;
                else
                    mode = Mode_HaltDI;
            }
            break;

        case 0x77: write_byte(hl(), a); break;
        case 0x78: a = b; break;
        case 0x79: a = c; break;
        case 0x7a: a = d; break;
        case 0x7b: a = e; break;
        case 0x7c: a = h; break;
        case 0x7d: a = l; break;
        case 0x7e: a = read_byte(hl()); break;
        case 0x7f: break; // LD A, A

        case 0x80: add_a_n(b); break;
        case 0x81: add_a_n(c); break;
        case 0x82: add_a_n(d); break;
        case 0x83: add_a_n(e); break;
        case 0x84: add_a_n(h); break;
        case 0x85: add_a_n(l); break;
        case 0x86: add_a_n(read_byte(hl())); break;
        case 0x87: add_a_n(a); break;

        case 0x88: adc_a_n(b); break;
        case 0x89: adc_a_n(c); break;
        case 0x8a: adc_a_n(d); break;
        case 0x8b: adc_a_n(e); break;
        case 0x8c: adc_a_n(h); break;
        case 0x8d: adc_a_n(l); break;
        case 0x8e: adc_a_n(read_byte(hl())); break;
        case 0x8f: adc_a_n(a); break;

        case 0x90: sub_a_n(b); break;
        case 0x91: sub_a_n(c); break;
        case 0x92: sub_a_n(d); break;
        case 0x93: sub_a_n(e); break;
        case 0x94: sub_a_n(h); break;
        case 0x95: sub_a_n(l); break;
        case 0x96: sub_a_n(read_byte(hl())); break;
        case 0x97: sub_a_n(a); break;

        case 0x98: sbc_a_n(b); break;
        case 0x99: sbc_a_n(c); break;
        case 0x9a: sbc_a_n(d); break;
        case 0x9b: sbc_a_n(e); break;
        case 0x9c: sbc_a_n(h); break;
        case 0x9d: sbc_a_n(l); break;
        case 0x9e: sbc_a_n(read_byte(hl())); break;
        case 0x9f: sbc_a_n(a); break;

        case 0xa0: and_a_n(b); break;
        case 0xa1: and_a_n(c); break;
        case 0xa2: and_a_n(d); break;
        case 0xa3: and_a_n(e); break;
        case 0xa4: and_a_n(h); break;
        case 0xa5: and_a_n(l); break;
        case 0xa6: and_a_n(read_byte(hl())); break;
        case 0xa7: and_a_n(a); break;

        case 0xa8: xor_a_n(b); break;
        case 0xa9: xor_a_n(c); break;
        case 0xaa: xor_a_n(d); break;
        case 0xab: xor_a_n(e); break;
        case 0xac: xor_a_n(h); break;
        case 0xad: xor_a_n(l); break;
        case 0xae: xor_a_n(read_byte(hl())); break;
        case 0xaf: xor_a_n(a); break;

        case 0xb0: or_a_n(b); break;
        case 0xb1: or_a_n(c); break;
        case 0xb2: or_a_n(d); break;
        case 0xb3: or_a_n(e); break;
        case 0xb4: or_a_n(h); break;
        case 0xb5: or_a_n(l); break;
        case 0xb6: or_a_n(read_byte(hl())); break;
        case 0xb7: or_a_n(a); break;

        case 0xb8: cp_a_n(b); break;
        case 0xb9: cp_a_n(c); break;
        case 0xba: cp_a_n(d); break;
        case 0xbb: cp_a_n(e); break;
        case 0xbc: cp_a_n(h); break;
        case 0xbd: cp_a_n(l); break;
        case 0xbe: cp_a_n(read_byte(hl())); break;
        case 0xbf: cp_a_n(a); break;

        case 0xc0:
            tick4();
            if (!(f & ZF))
                ret();
            break;
        case 0xc1:
            pop_rr(b, c);
            break;
        case 0xc2:
            if (f & ZF) {
                skip_operand();
                skip_operand();
            }
            else
                jp_nn();
            break;
        case 0xc3:
            jp_nn();
            break;
        case 0xc4:
            if (f & ZF) {
                skip_operand();
                skip_operand();
            }
            else
                call_nn();
            break;
        case 0xc5:
            push_rr(b, c);
            break;
        case 0xc6:
            add_a_n(read_operand());
            break;
        case 0xc7:
            rst_n(0x00);
            break;
        case 0xc8:
            tick4();

            if (f & ZF)
                ret();
            break;
        case 0xc9:
            ret();
            break;
        case 0xca:
            if (f & ZF)
                jp_nn();
            else {
                skip_operand();
                skip_operand();
            }
            break;
        case 0xcb:
            instr = read_operand();

            switch (instr) {
            case 0x00: rlc_r(b); break;
            case 0x01: rlc_r(c); break;
            case 0x02: rlc_r(d); break;
            case 0x03: rlc_r(e); break;
            case 0x04: rlc_r(h); break;
            case 0x05: rlc_r(l); break;
            case 0x06:
                {
                    byte value = read_byte(hl());
                    rlc_r(value);
                    write_byte(hl(), value);
                }
                break;
            case 0x07: rlc_r(a); break;

            case 0x08: rrc_r(b); break;
            case 0x09: rrc_r(c); break;
            case 0x0a: rrc_r(d); break;
            case 0x0b: rrc_r(e); break;
            case 0x0c: rrc_r(h); break;
            case 0x0d: rrc_r(l); break;
            case 0x0e:
                {
                    byte value = read_byte(hl());
                    rrc_r(value);
                    write_byte(hl(), value);
                }
                break;
            case 0x0f: rrc_r(a); break;

            case 0x10: rl_r(b); break;
            case 0x11: rl_r(c); break;
            case 0x12: rl_r(d); break;
            case 0x13: rl_r(e); break;
            case 0x14: rl_r(h); break;
            case 0x15: rl_r(l); break;
            case 0x16:
                {
                    byte value = read_byte(hl());
                    rl_r(value);
                    write_byte(hl(), value);
                }
                break;
            case 0x17: rl_r(a); break;

            case 0x18: rr_r(b); break;
            case 0x19: rr_r(c); break;
            case 0x1a: rr_r(d); break;
            case 0x1b: rr_r(e); break;
            case 0x1c: rr_r(h); break;
            case 0x1d: rr_r(l); break;
            case 0x1e:
                {
                    byte value = read_byte(hl());
                    rr_r(value);
                    write_byte(hl(), value);
                }
                break;
            case 0x1f: rr_r(a); break;

            case 0x20: sla_r(b); break;
            case 0x21: sla_r(c); break;
            case 0x22: sla_r(d); break;
            case 0x23: sla_r(e); break;
            case 0x24: sla_r(h); break;
            case 0x25: sla_r(l); break;
            case 0x26:
                {
                    byte value = read_byte(hl());
                    sla_r(value);
                    write_byte(hl(), value);
                }
                break;
            case 0x27: sla_r(a); break;

            case 0x28: sra_r(b); break;
            case 0x29: sra_r(c); break;
            case 0x2a: sra_r(d); break;
            case 0x2b: sra_r(e); break;
            case 0x2c: sra_r(h); break;
            case 0x2d: sra_r(l); break;
            case 0x2e:
                {
                    byte value = read_byte(hl());
                    sra_r(value);
                    write_byte(hl(), value);
                }
                break;
            case 0x2f: sra_r(a); break;

            case 0x30: swap_r(b); break;
            case 0x31: swap_r(c); break;
            case 0x32: swap_r(d); break;
            case 0x33: swap_r(e); break;
            case 0x34: swap_r(h); break;
            case 0x35: swap_r(l); break;
            case 0x36:
                {
                    byte value = read_byte(hl());
                    swap_r(value);
                    write_byte(hl(), value);
                }
                break;
            case 0x37: swap_r(a); break;

            case 0x38: srl_r(b); break;
            case 0x39: srl_r(c); break;
            case 0x3a: srl_r(d); break;
            case 0x3b: srl_r(e); break;
            case 0x3c: srl_r(h); break;
            case 0x3d: srl_r(l); break;
            case 0x3e:
                {
                    byte value = read_byte(hl());
                    srl_r(value);
                    write_byte(hl(), value);
                }
                break;
            case 0x3f: srl_r(a); break;

            case 0x40: bit_n(b, 0); break;
            case 0x41: bit_n(c, 0); break;
            case 0x42: bit_n(d, 0); break;
            case 0x43: bit_n(e, 0); break;
            case 0x44: bit_n(h, 0); break;
            case 0x45: bit_n(l, 0); break;
            case 0x46: bit_n(read_byte(hl()), 0); break;
            case 0x47: bit_n(a, 0); break;

            case 0x48: bit_n(b, 1); break;
            case 0x49: bit_n(c, 1); break;
            case 0x4a: bit_n(d, 1); break;
            case 0x4b: bit_n(e, 1); break;
            case 0x4c: bit_n(h, 1); break;
            case 0x4d: bit_n(l, 1); break;
            case 0x4e: bit_n(read_byte(hl()), 1); break;
            case 0x4f: bit_n(a, 1); break;

            case 0x50: bit_n(b, 2); break;
            case 0x51: bit_n(c, 2); break;
            case 0x52: bit_n(d, 2); break;
            case 0x53: bit_n(e, 2); break;
            case 0x54: bit_n(h, 2); break;
            case 0x55: bit_n(l, 2); break;
            case 0x56: bit_n(read_byte(hl()), 2); break;
            case 0x57: bit_n(a, 2); break;

            case 0x58: bit_n(b, 3); break;
            case 0x59: bit_n(c, 3); break;
            case 0x5a: bit_n(d, 3); break;
            case 0x5b: bit_n(e, 3); break;
            case 0x5c: bit_n(h, 3); break;
            case 0x5d: bit_n(l, 3); break;
            case 0x5e: bit_n(read_byte(hl()), 3); break;
            case 0x5f: bit_n(a, 3); break;

            case 0x60: bit_n(b, 4); break;
            case 0x61: bit_n(c, 4); break;
            case 0x62: bit_n(d, 4); break;
            case 0x63: bit_n(e, 4); break;
            case 0x64: bit_n(h, 4); break;
            case 0x65: bit_n(l, 4); break;
            case 0x66: bit_n(read_byte(hl()), 4); break;
            case 0x67: bit_n(a, 4); break;

            case 0x68: bit_n(b, 5); break;
            case 0x69: bit_n(c, 5); break;
            case 0x6a: bit_n(d, 5); break;
            case 0x6b: bit_n(e, 5); break;
            case 0x6c: bit_n(h, 5); break;
            case 0x6d: bit_n(l, 5); break;
            case 0x6e: bit_n(read_byte(hl()), 5); break;
            case 0x6f: bit_n(a, 5); break;

            case 0x70: bit_n(b, 6); break;
            case 0x71: bit_n(c, 6); break;
            case 0x72: bit_n(d, 6); break;
            case 0x73: bit_n(e, 6); break;
            case 0x74: bit_n(h, 6); break;
            case 0x75: bit_n(l, 6); break;
            case 0x76: bit_n(read_byte(hl()), 6); break;
            case 0x77: bit_n(a, 6); break;

            case 0x78: bit_n(b, 7); break;
            case 0x79: bit_n(c, 7); break;
            case 0x7a: bit_n(d, 7); break;
            case 0x7b: bit_n(e, 7); break;
            case 0x7c: bit_n(h, 7); break;
            case 0x7d: bit_n(l, 7); break;
            case 0x7e: bit_n(read_byte(hl()), 7); break;
            case 0x7f: bit_n(a, 7); break;

            case 0x80: res_r(b, 0); break;
            case 0x81: res_r(c, 0); break;
            case 0x82: res_r(d, 0); break;
            case 0x83: res_r(e, 0); break;
            case 0x84: res_r(h, 0); break;
            case 0x85: res_r(l, 0); break;
            case 0x86: res_hlp(0);  break;
            case 0x87: res_r(a, 0); break;

            case 0x88: res_r(b, 1); break;
            case 0x89: res_r(c, 1); break;
            case 0x8a: res_r(d, 1); break;
            case 0x8b: res_r(e, 1); break;
            case 0x8c: res_r(h, 1); break;
            case 0x8d: res_r(l, 1); break;
            case 0x8e: res_hlp(1);  break;
            case 0x8f: res_r(a, 1); break;

            case 0x90: res_r(b, 2); break;
            case 0x91: res_r(c, 2); break;
            case 0x92: res_r(d, 2); break;
            case 0x93: res_r(e, 2); break;
            case 0x94: res_r(h, 2); break;
            case 0x95: res_r(l, 2); break;
            case 0x96: res_hlp(2);  break;
            case 0x97: res_r(a, 2); break;

            case 0x98: res_r(b, 3); break;
            case 0x99: res_r(c, 3); break;
            case 0x9a: res_r(d, 3); break;
            case 0x9b: res_r(e, 3); break;
            case 0x9c: res_r(h, 3); break;
            case 0x9d: res_r(l, 3); break;
            case 0x9e: res_hlp(3);  break;
            case 0x9f: res_r(a, 3); break;

            case 0xa0: res_r(b, 4); break;
            case 0xa1: res_r(c, 4); break;
            case 0xa2: res_r(d, 4); break;
            case 0xa3: res_r(e, 4); break;
            case 0xa4: res_r(h, 4); break;
            case 0xa5: res_r(l, 4); break;
            case 0xa6: res_hlp(4);  break;
            case 0xa7: res_r(a, 4); break;

            case 0xa8: res_r(b, 5); break;
            case 0xa9: res_r(c, 5); break;
            case 0xaa: res_r(d, 5); break;
            case 0xab: res_r(e, 5); break;
            case 0xac: res_r(h, 5); break;
            case 0xad: res_r(l, 5); break;
            case 0xae: res_hlp(5);  break;
            case 0xaf: res_r(a, 5); break;

            case 0xb0: res_r(b, 6); break;
            case 0xb1: res_r(c, 6); break;
            case 0xb2: res_r(d, 6); break;
            case 0xb3: res_r(e, 6); break;
            case 0xb4: res_r(h, 6); break;
            case 0xb5: res_r(l, 6); break;
            case 0xb6: res_hlp(6);  break;
            case 0xb7: res_r(a, 6); break;

            case 0xb8: res_r(b, 7); break;
            case 0xb9: res_r(c, 7); break;
            case 0xba: res_r(d, 7); break;
            case 0xbb: res_r(e, 7); break;
            case 0xbc: res_r(h, 7); break;
            case 0xbd: res_r(l, 7); break;
            case 0xbe: res_hlp(7);  break;
            case 0xbf: res_r(a, 7); break;

            case 0xc0: set_r(b, 0); break;
            case 0xc1: set_r(c, 0); break;
            case 0xc2: set_r(d, 0); break;
            case 0xc3: set_r(e, 0); break;
            case 0xc4: set_r(h, 0); break;
            case 0xc5: set_r(l, 0); break;
            case 0xc6: set_hlp(0);  break;
            case 0xc7: set_r(a, 0); break;

            case 0xc8: set_r(b, 1); break;
            case 0xc9: set_r(c, 1); break;
            case 0xca: set_r(d, 1); break;
            case 0xcb: set_r(e, 1); break;
            case 0xcc: set_r(h, 1); break;
            case 0xcd: set_r(l, 1); break;
            case 0xce: set_hlp(1);  break;
            case 0xcf: set_r(a, 1); break;

            case 0xd0: set_r(b, 2); break;
            case 0xd1: set_r(c, 2); break;
            case 0xd2: set_r(d, 2); break;
            case 0xd3: set_r(e, 2); break;
            case 0xd4: set_r(h, 2); break;
            case 0xd5: set_r(l, 2); break;
            case 0xd6: set_hlp(2);  break;
            case 0xd7: set_r(a, 2); break;

            case 0xd8: set_r(b, 3); break;
            case 0xd9: set_r(c, 3); break;
            case 0xda: set_r(d, 3); break;
            case 0xdb: set_r(e, 3); break;
            case 0xdc: set_r(h, 3); break;
            case 0xdd: set_r(l, 3); break;
            case 0xde: set_hlp(3);  break;
            case 0xdf: set_r(a, 3); break;

            case 0xe0: set_r(b, 4); break;
            case 0xe1: set_r(c, 4); break;
            case 0xe2: set_r(d, 4); break;
            case 0xe3: set_r(e, 4); break;
            case 0xe4: set_r(h, 4); break;
            case 0xe5: set_r(l, 4); break;
            case 0xe6: set_hlp(4);  break;
            case 0xe7: set_r(a, 4); break;

            case 0xe8: set_r(b, 5); break;
            case 0xe9: set_r(c, 5); break;
            case 0xea: set_r(d, 5); break;
            case 0xeb: set_r(e, 5); break;
            case 0xec: set_r(h, 5); break;
            case 0xed: set_r(l, 5); break;
            case 0xee: set_hlp(5);  break;
            case 0xef: set_r(a, 5); break;

            case 0xf0: set_r(b, 6); break;
            case 0xf1: set_r(c, 6); break;
            case 0xf2: set_r(d, 6); break;
            case 0xf3: set_r(e, 6); break;
            case 0xf4: set_r(h, 6); break;
            case 0xf5: set_r(l, 6); break;
            case 0xf6: set_hlp(6);  break;
            case 0xf7: set_r(a, 6); break;

            case 0xf8: set_r(b, 7); break;
            case 0xf9: set_r(c, 7); break;
            case 0xfa: set_r(d, 7); break;
            case 0xfb: set_r(e, 7); break;
            case 0xfc: set_r(h, 7); break;
            case 0xfd: set_r(l, 7); break;
            case 0xfe: set_hlp(7);  break;
            case 0xff: set_r(a, 7); break;
            }
            break;
        case 0xcc:
            if (f & ZF)
                call_nn();
            else {
                skip_operand();
                skip_operand();
            }
            break;
        case 0xcd:
            call_nn();
            break;
        case 0xce:
            adc_a_n(read_operand());
            break;
        case 0xcf:
            rst_n(0x08);
            break;
        case 0xd0:
            tick4();

            if (!(f & CF))
                ret();
            break;
        case 0xd1:
            pop_rr(d, e);
            break;
        case 0xd2:
            if (f & CF) {
                skip_operand();
                skip_operand();
            }
            else
                jp_nn();
            break;
        case 0xd3: invalid_op(); break;
        case 0xd4:
            if (f & CF) {
                skip_operand();
                skip_operand();
            }
            else
                call_nn();
            break;
        case 0xd5:
            push_rr(d, e);
            break;
        case 0xd6:
            sub_a_n(read_operand());
            break;
        case 0xd7:
            rst_n(0x10);
            break;
        case 0xd8:
            tick4();

            if (f & CF)
                ret();
            break;
        case 0xd9:
            IME = 1;
            ret();
            break;
        case 0xda:
            if (f & CF)
                jp_nn();
            else {
                skip_operand();
                skip_operand();
            }
            break;
        case 0xdb: invalid_op(); break;
        case 0xdc:
            if (f & CF)
                call_nn();
            else {
                skip_operand();
                skip_operand();
            }
            break;
        case 0xdd: invalid_op(); break;
        case 0xde:
            sbc_a_n(read_operand());
            break;
        case 0xdf:
            rst_n(0x18);
            break;
        case 0xe0:
            write_byte(0xFF00 + read_operand(), a);
            break;
        case 0xe1:
            pop_rr(h, l);
            break;
        case 0xe2:
            write_byte(0xFF00+c, a);
            break;
        case 0xe3: invalid_op(); break;
        case 0xe4: invalid_op(); break;
        case 0xe5:
            push_rr(h, l);
            break;
        case 0xe6:
            and_a_n(read_operand());
            break;
        case 0xe7:
            rst_n(0x20);
            break;
        case 0xe8:
            sp = add_sp_n();
            tick4();
            break;
        case 0xe9:
            pc = hl();
            break;
        case 0xea:
            {
                byte low  = read_operand();
                byte high = read_operand();

                write_byte(high << 8 | low, a);
            }
            break;
        case 0xeb: invalid_op(); break;
        case 0xec: invalid_op(); break;
        case 0xed: invalid_op(); break;
        case 0xee:
            xor_a_n(read_operand());
            break;
        case 0xef:
            rst_n(0x28);
            break;
        case 0xf0: // FF_READ
            a = read_byte(0xFF00 + read_operand());
            break;
        case 0xf1:
            pop_rr(a, f);
            f &= 0xF0;
            break;
        case 0xf2: // FF_READ
            a = read_byte(0xFF00 + c);
            break;
        case 0xf3:
            IME = 0;
            break;
        case 0xf4: invalid_op(); break;
        case 0xf5:
            push_rr(a, f);
            break;
        case 0xf6:
            or_a_n(read_operand());
            break;
        case 0xf7:
            rst_n(0x30);
            break;
        case 0xf8:
            write_hl(add_sp_n());
            break;
        case 0xf9:
            sp = hl();
            tick4();
            break;
        case 0xfa:
            {
                byte low  = read_operand();
                byte high = read_operand();

                a = read_byte(high << 8 | low);
            }
            break;
        case 0xfb:
            mode = Mode_EnableIME;
            break;
        case 0xfc: invalid_op(); break;
        case 0xfd: invalid_op(); break;
        case 0xfe:
            cp_a_n(read_operand());
            break;
        case 0xff:
            rst_n(0x38);
            break;
    }
}
