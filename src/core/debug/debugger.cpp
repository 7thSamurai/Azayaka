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

#include "debugger.hpp"
#include "cpu/cpu.hpp"
#include "memory/mmu.hpp"
#include "rom/cart.hpp"
#include "rom/rom.hpp"
#include "gameboy.hpp"
#include "common/utils.hpp"

#include <sstream>

Debugger::Debugger(GameBoy *gb) {
    this->gb = gb;
    activated = 0;

    add_command(&Debugger::command_break, "\t\tAdds a breakpoint", 1, "break", "b");
    add_command(&Debugger::command_cartridge, "\tPrints information about the cartridge", 0, "cartrige", "cart");
    add_command(&Debugger::command_continue, "\t\tRuns until the next breakpoint or watchpoint", 0, "continue", "c");
    add_command(&Debugger::command_delete, "\t\tRemoves a breakpoint, or all breakpoints", -1, "delete", "d");
    add_command(&Debugger::command_disassemble, "\tDisassemble an instruction", 1, "disassemble", "dis");
    add_command(&Debugger::command_help, "\t\tPrints a help message", -1, "help", "h");
    add_command(&Debugger::command_lcd, "\t\t\tPrints information about the LCD", 0, "lcd");
    add_command(&Debugger::command_list, "\t\tLists the breakpoints and watchpoints", 0, "list", "l");
    add_command(&Debugger::command_print, "\t\tPrints a value", 1, "print", "p");
    add_command(&Debugger::command_quit, "\t\tQuits the emualator", 0, "quit", "q");
    add_command(&Debugger::command_registers, "\tPrints the values of the registers", 0, "registers", "reg");
    add_command(&Debugger::command_step, "\t\tRuns the next instruction", 0, "step", "s");
    add_command(&Debugger::command_unwatch, "\t\tRemoves a watchpoint, or all watchpoints", -1, "unwatch", "u");
    add_command(&Debugger::command_watch, "\t\tAdds a watchpoiint", 1, "watch", "w");

    io_addrs["P1"]    = 0xFF00;
    io_addrs["JOYP"]  = 0xFF00;
    io_addrs["SB"]    = 0xFF01;
    io_addrs["SC"]    = 0xFF02;
    io_addrs["DIV"]   = 0xFF04;
    io_addrs["TIMA"]  = 0xFF05;
    io_addrs["TMA"]   = 0xFF06;
    io_addrs["TAC"]   = 0xFF07;
    io_addrs["NR10"]  = 0xFF10;
    io_addrs["NR11"]  = 0xFF11;
    io_addrs["NR12"]  = 0xFF12;
    io_addrs["NR13"]  = 0xFF13;
    io_addrs["NR14"]  = 0xFF14;
    io_addrs["NR21"]  = 0xFF16;
    io_addrs["NR22"]  = 0xFF17;
    io_addrs["NR23"]  = 0xFF18;
    io_addrs["NR24"]  = 0xFF19;
    io_addrs["NR30"]  = 0xFF1A;
    io_addrs["NR31"]  = 0xFF1B;
    io_addrs["NR32"]  = 0xFF1C;
    io_addrs["NR33"]  = 0xFF1D;
    io_addrs["NR34"]  = 0xFF1E;
    io_addrs["NR41"]  = 0xFF20;
    io_addrs["NR42"]  = 0xFF21;
    io_addrs["NR43"]  = 0xFF22;
    io_addrs["NR44"]  = 0xFF23;
    io_addrs["NR50"]  = 0xFF24;
    io_addrs["NR51"]  = 0xFF25;
    io_addrs["NR52"]  = 0xFF26;
    io_addrs["LCDC"]  = 0xFF40;
    io_addrs["STAT"]  = 0xFF41;
    io_addrs["SCY"]   = 0xFF42;
    io_addrs["SCX"]   = 0xFF43;
    io_addrs["LY"]    = 0xFF44;
    io_addrs["LYC"]   = 0xFF45;
    io_addrs["DMA"]   = 0xFF46;
    io_addrs["BGP"]   = 0xFF47;
    io_addrs["OBP0"]  = 0xFF48;
    io_addrs["OBP1"]  = 0xFF49;
    io_addrs["WY"]    = 0xFF4A;
    io_addrs["WX"]    = 0xFF4B;
    io_addrs["KEY1"]  = 0xFF4D;
    io_addrs["VBK"]   = 0xFF4F;
    io_addrs["HDMA1"] = 0xFF51;
    io_addrs["HDMA2"] = 0xFF52;
    io_addrs["HDMA3"] = 0xFF53;
    io_addrs["HDMA4"] = 0xFF54;
    io_addrs["HDMA5"] = 0xFF55;
    io_addrs["RP"]    = 0xFF56;
    io_addrs["BCPS"]  = 0xFF68;
    io_addrs["BGPI"]  = 0xFF68;
    io_addrs["BCPD"]  = 0xFF69;
    io_addrs["BGPD"]  = 0xFF69;
    io_addrs["OCPS"]  = 0xFF6A;
    io_addrs["OBPI"]  = 0xFF6A;
    io_addrs["OCPD"]  = 0xFF6B;
    io_addrs["OBPD"]  = 0xFF6B;
    io_addrs["SVBK"]  = 0xFF70;
    io_addrs["IF"]    = 0xFF0F;
    io_addrs["IE"]    = 0xFFFF;
}

Debugger::~Debugger() {
}

void Debugger::run_command(const std::string &command) {
    std::vector <std::string> tokens;
    tokenize(command, tokens);

    auto it = commands.find(tokens.at(0));

    if (it != commands.end()) {
        if (it->second.num_of_args == -1 || it->second.num_of_args == tokens.size()-1)
            (this->*it->second.func)(tokens);
        else
            print("Invalid number of arguments!");
    }
    else
        print("Invalid Command!");
}

bool Debugger::update() {
    if (activated)
        return 0;

    cpu_debugger.step(gb->cpu);
    int address = cpu_debugger.check_pc(gb->cpu);

    if (address != -1) {
        print("Breakpoint triggered: $" + hex(address, 4));

        activated = 1;
    }

    if (memory_debugger.update_watchpoints(gb->mmu))
        activated = 1;

    if (activated)
        return 1;

    return gb->is_frame_done() ? 1 : 0;
}

int Debugger::get_reg8(char r) const {
    return gb->cpu->get_reg8(r);
}

int Debugger::get_reg16(char r1, char r2) const {
    return gb->cpu->get_reg16(r1, r2);
}

byte Debugger::get_memory8(word address) const {
    return gb->mmu->read_byte(address);
}

void Debugger::add_breakpoint(word address) {
    cpu_debugger.add_breakpoint(address);
}

void Debugger::add_watchpoint(word address) {
    memory_debugger.add_watchpoint(address, gb->mmu);
}

void Debugger::remove_breakpoint(word address) {
    cpu_debugger.remove_breakpoint(address);
}

void Debugger::remove_watchpoint(word address) {
    memory_debugger.remove_watchpoint(address);
}

void Debugger::clear_breakpoints() {
    cpu_debugger.clear_breakpoints();
}

void Debugger::clear_watchpoints() {
    memory_debugger.clear_watchpoints();
}

void Debugger::get_breakpoints(std::vector <word> &breakpoints) {
    cpu_debugger.get_breakpoints(breakpoints);
}

void Debugger::get_watchpoints(std::vector <word> &watchpoints) {
    memory_debugger.get_watchpoints(watchpoints);
}

int Debugger::disassemble_instr(word address, std::string &instr) {
    return disassembler.disassemble_byte(gb->mmu, address, instr);
}

int Debugger::get_rom_usage(word address) {
    return gb->rom->get_rom_usage(address);
}

std::string Debugger::get_rom_type() const {
    return gb->rom->get_rom_type();
}

std::string Debugger::get_rom_size() const {
    return gb->rom->get_rom_size();
}

std::string Debugger::get_ram_size() const {
    return gb->rom->get_ram_size();
}

bool Debugger::rom_is_mbc() const {
    return gb->rom->is_mbc();
}

bool Debugger::rom_has_bat() const {
    return gb->rom->has_bat();
}

bool Debugger::rom_has_rtc() const {
    return gb->rom->has_rtc();
}

bool Debugger::rom_has_rumble() const {
    return gb->rom->has_rumble();
}

bool Debugger::get_mbc_ram_on() const {
    return gb->rom->get_mbc_ram_on();
}

int Debugger::get_mbc_rom_bank() const {
    return gb->rom->get_mbc_rom_bank();
}

int Debugger::get_mbc_ram_bank() const {
    return gb->rom->get_mbc_ram_bank();
}

bool Debugger::rom_is_mbc1() const {
    return gb->rom->is_mbc1();
}

bool Debugger::get_mbc1_mode() const {
    return gb->rom->get_mbc1_mode();
}

bool Debugger::is_cgb() const {
    return gb->gbc_mode;
}

bool Debugger::is_activated() const {
    return activated;
}

void Debugger::set_activated(bool activated) {
    this->activated = activated;
}

void Debugger::add_command(CommandFunc func, const std::string &help_msg, int num_of_args, const std::string &command, const std::string &alias) {
    Command c = {
        func,
        num_of_args,
        command,
        alias,
        help_msg
    };

    commands[command] = c;

    if (alias != "")
        commands[alias] = c;
}

void Debugger::tokenize(const std::string &line, std::vector <std::string> &tokens) const {
    tokens.clear();

    std::stringstream ss;
    std::string token;
    ss << line;

    while (!ss.eof()) {
        ss >> token;

        tokens.push_back(token);
    }
}


int Debugger::get_reg(const std::string &reg) const {
    // 8-bit reg
    if (reg.size() == 1)
        return get_reg8(reg[0]);

    // 16-bit reg
    else if (reg.size() == 2)
        return get_reg16(reg[0], reg[1]);

    return -1;
}

int Debugger::get_num(const std::string &num) const {
    if (num[0] == '$') {
        if (is_a_num16(num.substr(1)))
            return std::stoi(num.substr(1), 0, 16);
        else
            return -1;
    }

    else if (num[0] == '0' && (num[1] == 'X' || num[1] == 'x')) {
        if (is_a_num16(num.substr(2)))
            return std::stoi(num.substr(2), 0, 16);
        else
            return -1;
    }

    else if (num[0] == '0' && (num[1] == 'B' || num[1] == 'b')) {
        if (is_a_num2(num.substr(2)))
            return std::stoi(num.substr(2), 0, 2);
        else
            return -1;
    }

    else {
        if (is_a_num10(num))
            return std::stoi(num);
        else
            return -1;
    }

    return -1;
}

int Debugger::get_io(const std::string &io) const {
    auto it = io_addrs.find(io);

    if (it == io_addrs.end())
        return -1;

    return it->second;
}

int Debugger::get_constant(const std::string &constant) const {
    int value;

    if ((constant.front() == '[' && constant.back() == ']') ||
        (constant.front() == '(' && constant.back() == ')'))
    {
        std::string c = constant.substr(1, constant.size()-2);

        if ((value = get_reg(c)) != -1)
            return get_memory8(value);

        else if ((value = get_num(c)) != -1)
            return get_memory8(value);

        else if ((value = get_io(c)) != -1)
            return get_memory8(value);
    }

    if ((value = get_reg(constant)) != -1)
        return value;

    else if ((value = get_num(constant)) != -1)
        return value;

    return -1;
}

void Debugger::print_help_msg(const Command &command) {
    if (command.alias == "")
        print(command.command + ": " + command.help_msg);
    else
        print(command.command + "(" + command.alias + "): " + command.help_msg);
}

#define INVALID_PARAMETER \
    print("Invalid parameter!"); \
    return;

#define STR(X) std::string(X)

void Debugger::command_break(const std::vector <std::string> &tokens) {
    int address = get_constant(tokens[1]);
    if (address == -1) {
        INVALID_PARAMETER
    }

    add_breakpoint(address);
}

void Debugger::command_cartridge(const std::vector <std::string> &tokens) {
    print("General:");
    print("    Type: " + get_rom_type());
    print("    Size: " + get_rom_size());

    if (get_ram_size() != "NONE") {
        print("\nRam: ");
        print("    Contains " + get_ram_size() + " of RAM");
        print("    Battery-Backed: " + STR(rom_has_bat() ? "Yes" : "No"));
    }

    else
        print("    No RAM");

    if (rom_is_mbc()) {
        print("\nMBC:");

        print("    Current ROM Bank: $" + hex(get_mbc_rom_bank(), 4));
        print("    Current RAM Bank: $" + hex(get_mbc_ram_bank(), 2));
        print("    RAM: " + STR(get_mbc_ram_on() ? "Enabled" : "Disabled"));

        if (rom_is_mbc1())
            print("    MBC1 Banking Mode: " + STR(get_mbc1_mode() ? "RAM" : "ROM"));

        print("\nFeatures:");
        print("    Has RTC:    " + STR(rom_has_rtc() ? "Yes" : "No"));
        print("    Has Rumble: " + STR(rom_has_rumble() ? "Yes" : "No"));
    }
}

void Debugger::command_continue(const std::vector <std::string> &tokens) {
    activated = 0;
}

void Debugger::command_delete(const std::vector <std::string> &tokens) {
    if (tokens.size() > 1) {
        int address = get_constant(tokens[1]);
        if (address == -1) {
            INVALID_PARAMETER
        }

        remove_breakpoint(address);
    }

    else
        clear_breakpoints();

}

void Debugger::command_disassemble(const std::vector <std::string> &tokens) {
    int address = get_constant(tokens[1]);
    if (address == -1) {
        INVALID_PARAMETER
    }

    std::string instr;
    disassemble_instr(address, instr);
    print(instr);
}

void Debugger::command_help(const std::vector <std::string> &tokens) {
    if (tokens.size() == 2) {
        auto it = commands.find(tokens[1]);

        if (it != commands.end())
            print_help_msg(it->second);
        else {
            INVALID_PARAMETER
        }
    }

    else if (tokens.size() == 1) {
        for (auto it : commands) {
            if (it.first != it.second.alias)
                print_help_msg(it.second);
        }
    }

    else {
        INVALID_PARAMETER
    }
}

void Debugger::command_lcd(const std::vector <std::string> &tokens) {
    int lcdc = get_memory8(0xFF40);
    int stat = get_memory8(0xFF41);

    int ly  = get_memory8(0xFF44);
    int lyc = get_memory8(0xFF45);

    const std::string modes[4] = {
        "Mode 0(H-Blank)",
        "Mode 1(V-Blank)",
        "Mode 2(OAM)",
        "Mode 3(VRAM)"
    };

    print("LCDC:");
    print("    LCD Enable: " + STR(lcdc & BIT7 ? "Enabled" : "Disabled"));
    print("    Window Tilemap: " + STR(lcdc & BIT6 ? "$9C00-$9FFF" : "$9800-$9BFF"));
    print("    Window: " + STR(lcdc & BIT5 ? "Enabled" : "Disabled"));
    print("    Background and Window Tileset: " + STR(lcdc & BIT4 ? "$8000-$8FFF" : "$8800-$97FF"));
    print("    Background Tilemap: " + STR(lcdc & BIT3 ? "$9C00-$9FFF" : "$9800-$9BFF"));
    print("    Sprite size: " + STR(lcdc & BIT2 ? "8x16" : "8x8"));
    print("    Sprites: " + STR(lcdc & BIT1 ? "Enabled" : "Disabled"));

    if (is_cgb())
        print("    Sprite Priority Flags: " + STR(lcdc & BIT0 ? "Enabled" : "Disabled"));
    else
        print("    Background: " + STR(lcdc & BIT0 ? "Enabled" : "Disabled"));

    print("\nSTAT:");
    print("    LYC Interrupt:     " + STR(stat & BIT6 ? "Enabled" : "Disabled"));
    print("    OAM Interrupt:     " + STR(stat & BIT5 ? "Enabled" : "Disabled"));
    print("    V-Blank Interrupt: " + STR(stat & BIT4 ? "Enabled" : "Disabled"));
    print("    H-Blank Interrupt: " + STR(stat & BIT3 ? "Enabled" : "Disabled"));
    print("    LY=LYC Flag: " + STR(stat & BIT2 ? "On" : "Off"));
    print("    Mode: " + modes[stat & 0b11]);

    print("\nLY:  $" + hex(ly, 2));
    print("LYC: $" + hex(lyc, 2));
}

void Debugger::command_list(const std::vector <std::string> &tokens) {
    std::vector <word> breakpoints;
    std::vector <word> watchpoints;

    get_breakpoints(breakpoints);
    get_watchpoints(watchpoints);

    if (breakpoints.size() != 0) {
        print("BreakPoints");
        for (word address : breakpoints)
            print("    $" + hex(address, 4));
    }

    if (watchpoints.size() != 0) {
        print("WatchPoints");
        for (word address : watchpoints)
            print("    $" + hex(address, 4));
    }
}

void Debugger::command_print(const std::vector <std::string> &tokens) {
    int value;

    if ((value = get_constant(tokens[1])) != -1)
        print("$" + hex(value));
    else {
        INVALID_PARAMETER
    }
}

void Debugger::command_quit(const std::vector <std::string> &tokens) {
    exit(0); // TODO
}

void Debugger::command_registers(const std::vector <std::string> &tokens) {
    int f = get_reg8('f');

    print("AF = $" + hex(get_reg16('a', 'f'), 4) + " ("
        + ((f & BIT7) ? 'Z' : '_')
        + ((f & BIT6) ? 'N' : '_')
        + ((f & BIT5) ? 'H' : '_')
        + ((f & BIT4) ? 'C' : '_') + ")");

    print("BC = $" + hex(get_reg16('b', 'c'), 4));
    print("DE = $" + hex(get_reg16('d', 'e'), 4));
    print("HL = $" + hex(get_reg16('h', 'l'), 4));
    print("PC = $" + hex(get_reg16('p', 'c'), 4));
    print("SP = $" + hex(get_reg16('s', 'p'), 4));
}

void Debugger::command_step(const std::vector <std::string> &tokens) {
    activated = 0;
    update();
    activated = 1;
}

void Debugger::command_unwatch(const std::vector <std::string> &tokens) {
    if (tokens.size() > 1) {
        int address = get_constant(tokens[1]);
        if (address == -1) {
            INVALID_PARAMETER
        }

        remove_watchpoint(address);
    }

    else
        clear_watchpoints();
}

void Debugger::command_watch(const std::vector <std::string> &tokens) {
    int address = get_constant(tokens[1]);
    if (address == -1) {
        INVALID_PARAMETER
    }

    add_watchpoint(address);
}


#undef INVALID_PARAMETER
#undef STR
