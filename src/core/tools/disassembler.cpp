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

#include "core/tools/disassembler.hpp"
#include "core/memory/mmu.hpp"
#include "common/string_utils.hpp"

#include <fstream>
#include <iostream>

#define N   "$"+hex(read_byte())
#define NN  "$"+hex(read_word())
#define IN  read_in(comment)
#define INN read_inn(comment)
#define REL read_branch()

#define A "a"
#define B "b"
#define C "c"
#define D "d"
#define E "e"
#define H "h"
#define L "l"
#define F "f"

#define BC "bc"
#define DE "de"
#define HL "hl"
#define AF "af"
#define SP "sp"

// Indirect
#define IC "($ff00+c)"
#define IBC "(bc)"
#define IDE "(de)"
#define IHL "(hl)"

#define USAGE_DATA        1
#define USAGE_INSTR_START 2
#define USAGE_INSTR       3

Disassembler::Disassembler() {
}

Disassembler::~Disassembler() {
}

std::string Disassembler::read_branch() {
    s8 offset = s8(read_byte());

    return "$" + hex(pc + offset);
}

std::string Disassembler::read_in(std::string &comment) {
    byte offset = read_byte();
    comment = read_io(0xFF00 + offset);

    return "($ff00+$" + hex(offset) + ")";
}

std::string Disassembler::read_inn(std::string &comment) {
    word address = read_word();
    comment = read_io(address);

    return "($" + hex(address) + ")";
}

std::string Disassembler::read_io(word address) {
    switch (address) {
        case 0xFF00: return "JOYP";
        case 0xFF01: return "SB";
        case 0xFF02: return "SC";
        case 0xFF04: return "DIV";
        case 0xFF05: return "TIMA";
        case 0xFF06: return "TMA";
        case 0xFF07: return "TAC";
        case 0xFF10: return "NR10";
        case 0xFF11: return "NR11";
        case 0xFF12: return "NR12";
        case 0xFF13: return "NR13";
        case 0xFF14: return "NR14";
        case 0xFF16: return "NR21";
        case 0xFF17: return "NR22";
        case 0xFF18: return "NR23";
        case 0xFF19: return "NR24";
        case 0xFF1A: return "NR30";
        case 0xFF1B: return "NR31";
        case 0xFF1C: return "NR32";
        case 0xFF1D: return "NR33";
        case 0xFF1E: return "NR34";
        case 0xFF20: return "NR41";
        case 0xFF21: return "NR42";
        case 0xFF22: return "NR43";
        case 0xFF23: return "NR44";
        case 0xFF24: return "NR50";
        case 0xFF25: return "NR51";
        case 0xFF26: return "NR52";
        case 0xFF40: return "LCDC";
        case 0xFF41: return "STAT";
        case 0xFF42: return "SCY";
        case 0xFF43: return "SCX";
        case 0xFF44: return "LY";
        case 0xFF45: return "LYC";
        case 0xFF46: return "DMA";
        case 0xFF47: return "BGP";
        case 0xFF48: return "OBP0";
        case 0xFF49: return "OBP1";
        case 0xFF4A: return "WY";
        case 0xFF4B: return "WX";
        case 0xFF4D: return "KEY1";
        case 0xFF4F: return "VBK";
        case 0xFF51: return "HDMA1";
        case 0xFF52: return "HDMA2";
        case 0xFF53: return "HDMA3";
        case 0xFF54: return "HDMA4";
        case 0xFF55: return "HDMA5";
        case 0xFF56: return "RP";
        case 0xFF68: return "BCPS/BGPI";
        case 0xFF69: return "BCPD/BGPD";
        case 0xFF6A: return "OCPS/OBPI";
        case 0xFF6B: return "OCPD/OBPD";
        case 0xFF70: return "SVBK";
        case 0xFF0F: return "IF";
        case 0xFFFF: return "IE";

        default:
            break;
    }

    return "";
}

word Disassembler::read_word() {
    byte lo = read_byte();
    byte hi = read_byte();

    return hi << 8 | lo;
}

std::string merge(const std::vector <std::string> &list, const std::string &seperator) {
    if (!list.size())
        return "";

    std::string out;

    for (unsigned int i = 0; i < list.size()-1; i++)
        out += list[i] + seperator;

    return out + list.back();
}

#define op(code, name, ...) \
    case code: \
        instr = std::string(name) + " " + merge(std::vector<std::string>{__VA_ARGS__}, ", "); \
        break;

int Disassembler::disassemble_instr(byte op_code, std::string &instr) {
    std::string comment = "";

    size = 0;

    switch (op_code) {
        op(0x00, "nop");
        op(0x01, "ld", BC, NN);
        op(0x02, "ld", IBC, A);
        op(0x03, "inc", BC);
        op(0x04, "inc", B);
        op(0x05, "dec", B);
        op(0x06, "ld", B, N);
        op(0x07, "rlca");
        op(0x08, "ld", INN, SP);
        op(0x09, "add", HL, BC);
        op(0x0a, "ld", A, IBC);
        op(0x0b, "dec", BC);
        op(0x0c, "inc", C);
        op(0x0d, "dec", C);
        op(0x0e, "ld", C, N);
        op(0x0f, "rrca");
        op(0x10, "stop");
        op(0x11, "ld", DE, NN);
        op(0x12, "ld", IDE, A);
        op(0x13, "inc", DE);
        op(0x14, "inc", D);
        op(0x15, "dec", D);
        op(0x16, "ld", D, N);
        op(0x17, "rla");
        op(0x18, "jr", REL);
        op(0x19, "add", HL, DE);
        op(0x1a, "ld", A, IDE);
        op(0x1b, "dec", DE);
        op(0x1c, "inc", E);
        op(0x1d, "dec", E);
        op(0x1e, "ld", E, N);
        op(0x1f, "rra");
        op(0x20, "jr", "nz", REL);
        op(0x21, "ld", HL, NN);
        op(0x22, "ldi", IHL, A);
        op(0x23, "inc", HL);
        op(0x24, "inc", H);
        op(0x25, "dec", H);
        op(0x26, "ld", H, N);
        op(0x27, "daa");
        op(0x28, "jr", "z", REL);
        op(0x29, "add", HL, HL);
        op(0x2a, "ldi", A, IHL);
        op(0x2b, "dec", HL);
        op(0x2c, "inc", L);
        op(0x2d, "dec", L);
        op(0x2e, "ld", L, N);
        op(0x2f, "cpl");
        op(0x30, "jr", "nc", REL);
        op(0x31, "ld", SP, NN);
        op(0x32, "ldd", IHL, A);
        op(0x33, "inc", SP);
        op(0x34, "inc", IHL);
        op(0x35, "dec", IHL);
        op(0x36, "ld", IHL, N);
        op(0x37, "scf");
        op(0x38, "jr", "c", REL);
        op(0x39, "add", HL, SP);
        op(0x3a, "ldd", A, IHL);
        op(0x3b, "dec", SP);
        op(0x3c, "inc", A);
        op(0x3d, "dec", A);
        op(0x3e, "ld", A, N);
        op(0x3f, "ccf");
        op(0x40, "ld", B, B);
        op(0x41, "ld", B, C);
        op(0x42, "ld", B, D);
        op(0x43, "ld", B, E);
        op(0x44, "ld", B, H);
        op(0x45, "ld", B, L);
        op(0x46, "ld", B, IHL);
        op(0x47, "ld", B, A);
        op(0x48, "ld", C, B);
        op(0x49, "ld", C, C);
        op(0x4a, "ld", C, D);
        op(0x4b, "ld", C, E);
        op(0x4c, "ld", C, H);;
        op(0x4d, "ld", C, L);
        op(0x4e, "ld", C, IHL);
        op(0x4f, "ld", C, A);
        op(0x50, "ld", D, B);
        op(0x51, "ld", D, C);
        op(0x52, "ld", D, D);
        op(0x53, "ld", D, E);
        op(0x54, "ld", D, H);
        op(0x55, "ld", D, L);
        op(0x56, "ld", D, IHL);
        op(0x57, "ld", D, A);
        op(0x58, "ld", E, B);
        op(0x59, "ld", E, C);
        op(0x5a, "ld", E, D);
        op(0x5b, "ld", E, E);
        op(0x5c, "ld", E, H);
        op(0x5d, "ld", E, L);
        op(0x5e, "ld", E, IHL);
        op(0x5f, "ld", E, A);
        op(0x60, "ld", H, B);
        op(0x61, "ld", H, C);
        op(0x62, "ld", H, D);
        op(0x63, "ld", H, E);
        op(0x64, "ld", H, H);
        op(0x65, "ld", H, L);
        op(0x66, "ld", H, IHL);
        op(0x67, "ld", H, A);
        op(0x68, "ld", L, B);
        op(0x69, "ld", L, C);
        op(0x6a, "ld", L, D);
        op(0x6b, "ld", L, E);
        op(0x6c, "ld", L, H);
        op(0x6d, "ld", L, L);
        op(0x6e, "ld", L, IHL);
        op(0x6f, "ld", L, A);
        op(0x70, "ld", IHL, B);
        op(0x71, "ld", IHL, C);
        op(0x72, "ld", IHL, D);
        op(0x73, "ld", IHL, E);
        op(0x74, "ld", IHL, H);
        op(0x75, "ld", IHL, L);
        op(0x76, "halt");
        op(0x77, "ld", IHL, A);
        op(0x78, "ld", A, B);
        op(0x79, "ld", A, C);
        op(0x7a, "ld", A, D);
        op(0x7b, "ld", A, E);
        op(0x7c, "ld", A, H);
        op(0x7d, "ld", A, L);
        op(0x7e, "ld", A, IHL);
        op(0x7f, "ld", A, A);
        op(0x80, "add", A, B);
        op(0x81, "add", A, C);
        op(0x82, "add", A, D);
        op(0x83, "add", A, E);
        op(0x84, "add", A, H);
        op(0x85, "add", A, L);
        op(0x86, "add", A, IHL);
        op(0x87, "add", A, A);
        op(0x88, "adc", A, B);
        op(0x89, "adc", A, C);
        op(0x8a, "adc", A, D);
        op(0x8b, "adc", A, E);
        op(0x8c, "adc", A, H);
        op(0x8d, "adc", A, L);
        op(0x8e, "adc", A, IHL);
        op(0x8f, "adc", A, A);
        op(0x90, "sub", A, B);
        op(0x91, "sub", A, C);
        op(0x92, "sub", A, D);
        op(0x93, "sub", A, E);
        op(0x94, "sub", A, H);
        op(0x95, "sub", A, L);
        op(0x96, "sub", A, IHL);
        op(0x97, "sub", A, A);
        op(0x98, "sbc", A, B);
        op(0x99, "sbc", A, C);
        op(0x9a, "sbc", A, D);
        op(0x9b, "sbc", A, E);
        op(0x9c, "sbc", A, H);
        op(0x9d, "sbc", A, L);
        op(0x9e, "sbc", A, IHL);
        op(0x9f, "sbc", A, A);
        op(0xa0, "and", A, B);
        op(0xa1, "and", A, C);
        op(0xa2, "and", A, D);
        op(0xa3, "and", A, E);
        op(0xa4, "and", A, H);
        op(0xa5, "and", A, L);
        op(0xa6, "and", A, IHL);
        op(0xa7, "and", A, A);
        op(0xa8, "xor", A, B);
        op(0xa9, "xor", A, C);
        op(0xaa, "xor", A, D);
        op(0xab, "xor", A, E);
        op(0xac, "xor", A, H);
        op(0xad, "xor", A, L);
        op(0xae, "xor", A, IHL);
        op(0xaf, "xor", A, A);
        op(0xb0, "or", A, B);
        op(0xb1, "or", A, C);
        op(0xb2, "or", A, D);
        op(0xb3, "or", A, E);
        op(0xb4, "or", A, H);
        op(0xb5, "or", A, L);
        op(0xb6, "or", A, IHL);
        op(0xb7, "or", A, A);
        op(0xb8, "cp", A, B);
        op(0xb9, "cp", A, C);
        op(0xba, "cp", A, D);
        op(0xbb, "cp", A, E);
        op(0xbc, "cp", A, H);
        op(0xbd, "cp", A, L);
        op(0xbe, "cp", A, IHL);
        op(0xbf, "cp", A, A);
        op(0xc0, "ret nz");
        op(0xc1, "pop", BC);
        op(0xc2, "jp", "nz", NN);
        op(0xc3, "jp", NN);
        op(0xc4, "call", "nz", NN);
        op(0xc5, "push", BC);
        op(0xc6, "add", A, N);
        op(0xc7, "rst 0");
        op(0xc8, "ret z");
        op(0xc9, "ret");
        op(0xca, "jp", "z", NN);
        case 0xcb:
            switch(read_byte()) {
            op(0x00, "rlc", B);
            op(0x01, "rlc", C);
            op(0x02, "rlc", D);
            op(0x03, "rlc", E);
            op(0x04, "rlc", H);
            op(0x05, "rlc", L);
            op(0x06, "rlc", IHL);
            op(0x07, "rlc", A);
            op(0x08, "rrc", B);
            op(0x09, "rrc", C);
            op(0x0a, "rrc", D);
            op(0x0b, "rrc", E);
            op(0x0c, "rrc", H);
            op(0x0d, "rrc", L);
            op(0x0e, "rrc", IHL);
            op(0x0f, "rrc", A);
            op(0x10, "rl", B);
            op(0x11, "rl", C);
            op(0x12, "rl", D);
            op(0x13, "rl", E);
            op(0x14, "rl", H);
            op(0x15, "rl", L);
            op(0x16, "rl", IHL);
            op(0x17, "rl", A);
            op(0x18, "rr", B);
            op(0x19, "rr", C);
            op(0x1a, "rr", D);
            op(0x1b, "rr", E);
            op(0x1c, "rr", H);
            op(0x1d, "rr", L);
            op(0x1e, "rr", IHL);
            op(0x1f, "rr", A);
            op(0x20, "sla", B);
            op(0x21, "sla", C);
            op(0x22, "sla", D);
            op(0x23, "sla", E);
            op(0x24, "sla", H);
            op(0x25, "sla", L);
            op(0x26, "sla", IHL);
            op(0x27, "sla", A);
            op(0x28, "sra", B);
            op(0x29, "sra", C);
            op(0x2a, "sra", D);
            op(0x2b, "sra", E);
            op(0x2c, "sra", H);
            op(0x2d, "sra", L);
            op(0x2e, "sra", IHL);
            op(0x2f, "sra", A);
            op(0x30, "swap", B);
            op(0x31, "swap", C);
            op(0x32, "swap", D); break;
            op(0x33, "swap", E);
            op(0x34, "swap", H);
            op(0x35, "swap", L);
            op(0x36, "swap", IHL);
            op(0x37, "swap", A);
            op(0x38, "srl", B);
            op(0x39, "srl", C);
            op(0x3a, "srl", D);
            op(0x3b, "srl", E);
            op(0x3c, "srl", H);
            op(0x3d, "srl", L);
            op(0x3e, "srl", IHL);
            op(0x3f, "srl", A);
            op(0x40, "bit", "0", B);
            op(0x41, "bit", "0", C);
            op(0x42, "bit", "0", D);
            op(0x43, "bit", "0", E);
            op(0x44, "bit", "0", H);
            op(0x45, "bit", "0", L);
            op(0x46, "bit", "0", IHL);
            op(0x47, "bit", "0", A);
            op(0x48, "bit", "1", B);
            op(0x49, "bit", "1", C);
            op(0x4a, "bit", "1", D);
            op(0x4b, "bit", "1", E);
            op(0x4c, "bit", "1", H);
            op(0x4d, "bit", "1", L);
            op(0x4e, "bit", "1", IHL);
            op(0x4f, "bit", "1", A);
            op(0x50, "bit", "2", B);
            op(0x51, "bit", "2", C);
            op(0x52, "bit", "2", D);
            op(0x53, "bit", "2", E);
            op(0x54, "bit", "2", H);
            op(0x55, "bit", "2", L);
            op(0x56, "bit", "2", IHL);
            op(0x57, "bit", "2", A);
            op(0x58, "bit", "3", B);
            op(0x59, "bit", "3", C);
            op(0x5a, "bit", "3", D);
            op(0x5b, "bit", "3", E);
            op(0x5c, "bit", "3", H);
            op(0x5d, "bit", "3", L);
            op(0x5e, "bit", "3", IHL);
            op(0x5f, "bit", "3", A);
            op(0x60, "bit", "4", B);
            op(0x61, "bit", "4", C);
            op(0x62, "bit", "4", D);
            op(0x63, "bit", "4", E);
            op(0x64, "bit", "4", H);
            op(0x65, "bit", "4", L);
            op(0x66, "bit", "4", IHL);
            op(0x67, "bit", "4", A);
            op(0x68, "bit", "5", B);
            op(0x69, "bit", "5", C);
            op(0x6a, "bit", "5", D);
            op(0x6b, "bit", "5", E);
            op(0x6c, "bit", "5", H);
            op(0x6d, "bit", "5", L);
            op(0x6e, "bit", "5", IHL);
            op(0x6f, "bit", "5", A);
            op(0x70, "bit", "6", B);
            op(0x71, "bit", "6", C);
            op(0x72, "bit", "6", D);
            op(0x73, "bit", "6", E);
            op(0x74, "bit", "6", H);
            op(0x75, "bit", "6", L);
            op(0x76, "bit", "6", IHL);
            op(0x77, "bit", "6", A);
            op(0x78, "bit", "7", B);
            op(0x79, "bit", "7", C);
            op(0x7a, "bit", "7", D);
            op(0x7b, "bit", "7", E);
            op(0x7c, "bit", "7", H);
            op(0x7d, "bit", "7", L);
            op(0x7e, "bit", "7", IHL);
            op(0x7f, "bit", "7", A);
            op(0x80, "res", "0", B);
            op(0x81, "res", "0", C);
            op(0x82, "res", "0", D);
            op(0x83, "res", "0", E);
            op(0x84, "res", "0", H);
            op(0x85, "res", "0", L);
            op(0x86, "res", "0", IHL);
            op(0x87, "res", "0", A);
            op(0x88, "res", "1", B);
            op(0x89, "res", "1", C);
            op(0x8a, "res", "1", D);
            op(0x8b, "res", "1", E);;
            op(0x8c, "res", "1", H);
            op(0x8d, "res", "1", L);
            op(0x8e, "res", "1", IHL);
            op(0x8f, "res", "1", A);
            op(0x90, "res", "2", B);
            op(0x91, "res", "2", C);
            op(0x92, "res", "2", D);
            op(0x93, "res", "2", E);
            op(0x94, "res", "2", H);
            op(0x95, "res", "2", L);
            op(0x96, "res", "2", IHL);
            op(0x97, "res", "2", A);
            op(0x98, "res", "3", B);
            op(0x99, "res", "3", C);
            op(0x9a, "res", "3", D);
            op(0x9b, "res", "3", E);
            op(0x9c, "res", "3", H);
            op(0x9d, "res", "3", L);
            op(0x9e, "res", "3", IHL);
            op(0x9f, "res", "3", A);
            op(0xa0, "res", "4", B);
            op(0xa1, "res", "4", C);
            op(0xa2, "res", "4", D);
            op(0xa3, "res", "4", E);
            op(0xa4, "res", "4", H);
            op(0xa5, "res", "4", L);
            op(0xa6, "res", "4", IHL);
            op(0xa7, "res", "4", A);
            op(0xa8, "res", "5", B);
            op(0xa9, "res", "5", C);
            op(0xaa, "res", "5", D);
            op(0xab, "res", "5", E);
            op(0xac, "res", "5", H);
            op(0xad, "res", "5", L);
            op(0xae, "res", "5", IHL);
            op(0xaf, "res", "5", A);
            op(0xb0, "res", "6", B);
            op(0xb1, "res", "6", C);
            op(0xb2, "res", "6", D);
            op(0xb3, "res", "6", E);
            op(0xb4, "res", "6", H);
            op(0xb5, "res", "6", L);
            op(0xb6, "res", "6", IHL);
            op(0xb7, "res", "6", A);
            op(0xb8, "res", "7", B);
            op(0xb9, "res", "7", C);
            op(0xba, "res", "7", D);
            op(0xbb, "res", "7", E);
            op(0xbc, "res", "7", H);
            op(0xbd, "res", "7", L);
            op(0xbe, "res", "7", IHL);
            op(0xbf, "res", "7", A);
            op(0xc0, "set", "0", B);
            op(0xc1, "set", "0", C);
            op(0xc2, "set", "0", D);
            op(0xc3, "set", "0", E);
            op(0xc4, "set", "0", H);
            op(0xc5, "set", "0", L);
            op(0xc6, "set", "0", IHL);
            op(0xc7, "set", "0", A);
            op(0xc8, "set", "1", B);
            op(0xc9, "set", "1", C);
            op(0xca, "set", "1", D);
            op(0xcb, "set", "1", E);
            op(0xcc, "set", "1", H);
            op(0xcd, "set", "1", L);
            op(0xce, "set", "1", IHL);
            op(0xcf, "set", "1", A);
            op(0xd0, "set", "2", B);
            op(0xd1, "set", "2", C);
            op(0xd2, "set", "2", D);
            op(0xd3, "set", "2", E);
            op(0xd4, "set", "2", H);
            op(0xd5, "set", "2", L);
            op(0xd6, "set", "2", IHL);
            op(0xd7, "set", "2", A);
            op(0xd8, "set", "3", B);
            op(0xd9, "set", "3", C);
            op(0xda, "set", "3", D);
            op(0xdb, "set", "3", E);
            op(0xdc, "set", "3", H);
            op(0xdd, "set", "3", L);
            op(0xde, "set", "3", IHL);
            op(0xdf, "set", "3", A);
            op(0xe0, "set", "4", B);
            op(0xe1, "set", "4", C);
            op(0xe2, "set", "4", D);
            op(0xe3, "set", "4", E);
            op(0xe4, "set", "4", H);
            op(0xe5, "set", "4", L);
            op(0xe6, "set", "4", IHL);
            op(0xe7, "set", "4", A);
            op(0xe8, "set", "5", B);
            op(0xe9, "set", "5", C);
            op(0xea, "set", "5", D);
            op(0xeb, "set", "5", E);
            op(0xec, "set", "5", H);
            op(0xed, "set", "5", L);
            op(0xee, "set", "5", IHL);
            op(0xef, "set", "5", A);
            op(0xf0, "set", "6", B);
            op(0xf1, "set", "6", C);
            op(0xf2, "set", "6", D);
            op(0xf3, "set", "6", E);
            op(0xf4, "set", "6", H);
            op(0xf5, "set", "6", L);
            op(0xf6, "set", "6", IHL);
            op(0xf7, "set", "6", A);
            op(0xf8, "set", "7", B);
            op(0xf9, "set", "7", C);
            op(0xfa, "set", "7", D);
            op(0xfb, "set", "7", E);
            op(0xfc, "set", "7", H);
            op(0xfd, "set", "7", L);
            op(0xfe, "set", "7", IHL);
            op(0xff, "set", "7", A);
            }
            break;
        op(0xcc, "call", "z", NN);
        op(0xcd, "call", NN);
        op(0xce, "adc", N);
        op(0xcf, "rst 08");
        op(0xd0, "ret nc");
        op(0xd1, "pop", DE);
        op(0xd2, "jp", "nc", NN);
        op(0xd3, "invalid"); return -1;
        op(0xd4, "call", "nc", NN);
        op(0xd5, "push", DE);
        op(0xd6, "sub", N);
        op(0xd7, "rst 10");
        op(0xd8, "ret", C);
        op(0xd9, "reti");
        op(0xda, "jp", C, NN);
        op(0xdb, "invalid"); return -1;
        op(0xdc, "call", C, NN);
        op(0xdd, "invalid"); return -1;
        op(0xde, "sbc", A, N);
        op(0xdf, "rst 18");
        op(0xe0, "ld", IN, A);
        op(0xe1, "pop", HL);
        op(0xe2, "ld", IC, A);
        op(0xe3, "invalid"); return -1;
        op(0xe4, "invalid"); return -1;
        op(0xe5, "push", HL);
        op(0xe6, "and", N);
        op(0xe7, "rst 20");
        op(0xe8, "add", SP, N);
        op(0xe9, "jp", IHL);
        op(0xea, "ld", INN, A);
        op(0xeb, "invalid"); return -1;
        op(0xec, "invalid"); return -1;
        op(0xed, "invalid"); return -1;
        op(0xee, "xor", N);
        op(0xef, "rst 28");
        op(0xf0, "ld", A, IN);
        op(0xf1, "pop", AF);
        op(0xf2, "ld", A, IC);
        op(0xf3, "di");
        op(0xf4, "invalid"); return -1;
        op(0xf5, "push", AF);
        op(0xf6, "or", N);
        op(0xf7, "rst 30");
        op(0xf8, "ldhl", SP, N); // TODO
        op(0xf9, "ld", SP, HL);
        op(0xfa, "ld", A, INN);
        op(0xfb, "ei");
        op(0xfc, "invalid"); return -1;
        op(0xfd, "invalid"); return -1;
        op(0xfe, "cp", N);
        op(0xff, "rst 38");
    }

    if (comment != "")
        instr += " ; " + comment;

    return size;
}

int FileDisassembler::disassemble(const std::string &file_path) {
    std::ifstream file(file_path, std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.is_open())
        return -1;

    unsigned int size = file.tellg();
    data = new byte[size];

    file.seekg(0, std::ios::beg);
    file.read((char*)data, size);
    file.close();

    pc = 0;
    std::string instr, pc_str;

    while (pc < size) {
        pc_str = "$" + hex(pc) + ":\t";
        disassemble_instr(read_byte(), instr);

        std::cout << pc_str << instr << std::endl;
    }

    delete[] data;

    return 0;
}

int FileDisassembler::disassemble(const std::string &file_path, const std::string &rom_usage_path) {
    std::ifstream file(file_path, std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.is_open())
        return -1;

    unsigned int size = file.tellg();
    data = new byte[size];

    file.seekg(0, std::ios::beg);
    file.read((char*)data, size);
    file.close();

    std::ifstream rom_usage_file(rom_usage_path, std::ios::in | std::ios::binary | std::ios::ate);
    if (!rom_usage_file.is_open())
        return -1;

    if (rom_usage_file.tellg() != size) {
        rom_usage_file.close();
        return -1;
    }

    byte *rom_usage = new byte[size];
    rom_usage_file.seekg(0, std::ios::beg);
    rom_usage_file.read((char*)rom_usage, size);
    rom_usage_file.close();

    pc = 0;
    std::string instr, pc_str;
    std::vector <byte> data_bytes;

    while (pc < size) {
        if (rom_usage[pc] == USAGE_INSTR_START) {
            if (data_bytes.size()) {
                print_data(data_bytes);
                data_bytes.clear();
            }

            pc_str =  "$" + hex(pc) + ":\t";
            disassemble_instr(read_byte(), instr);
            std::cout << pc_str << instr << std::endl;
        }

        else if (rom_usage[pc] == USAGE_DATA) {
            if (!data_bytes.size())
                std::cout << "$" << hex(pc) << ":" << std::endl;

            data_bytes.push_back(data[pc++]);
        }

        else if (rom_usage[pc] == USAGE_INSTR) {
            std::cout << "ERROR: Instruction Operand" << std::endl;
            pc++;
        }

        else {
            if (data_bytes.size()) {
                print_data(data_bytes);
                data_bytes.clear();
            }

            pc++;
        }
    }

    if (data_bytes.size())
        print_data(data_bytes);

    delete[] data;
    delete[] rom_usage;

    return 0;
}

byte FileDisassembler::read_byte() {
    size++;
    return data[pc++];
}

void FileDisassembler::print_data(const std::vector <byte> &data_bytes) {
    for (unsigned int i = 0; i < data_bytes.size(); i += 16) {
        std::cout << "\t";

        for (unsigned int j = 0; j < 16; j++) {
            if (i+j > data_bytes.size())
                break;

            std::cout << "$" << hex(data_bytes[i+j]) << " ";
        }

        std::cout << std::endl;
    }
}


int MemoryDisassembler::disassemble_byte(Mmu *mmu, word address, std::string &instr) {
    this->mmu = mmu;
    pc = address;

    return disassemble_instr(read_byte(), instr);
}

byte MemoryDisassembler::read_byte() {
    size++;
    return mmu->read_byte(pc++);
}
