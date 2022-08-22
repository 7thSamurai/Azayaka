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

#include "cpu_debugger.hpp"
#include "memory_debugger.hpp"
#include "tools/disassembler.hpp"

#include <string>
#include <vector>
#include <map>
#include <iostream>

class GameBoy;
class Debugger;

typedef void (Debugger::*CommandFunc)(const std::vector <std::string> &tokens);

class Debugger
{
public:
    Debugger(GameBoy *gb);
    ~Debugger();

    void run_command(const std::string &command);

    // Returns true if time to render frame
    bool update();

    int get_reg8(char r) const;
    int get_reg16(char r1, char r2) const;

    byte get_memory8(word address) const;

    void add_breakpoint(word address);
    void add_watchpoint(word address);

    void remove_breakpoint(word address);
    void remove_watchpoint(word address);

    void clear_breakpoints();
    void clear_watchpoints();

    void get_breakpoints(std::vector <word> &breakpoints);
    void get_watchpoints(std::vector <word> &watchpoints);

    int disassemble_instr(word address, std::string &instr);

    int get_rom_usage(word address);

    std::string get_rom_type() const;
    std::string get_rom_size() const;
    std::string get_ram_size() const;

    bool rom_is_mbc    () const;
    bool rom_has_bat   () const;
    bool rom_has_rtc   () const;
    bool rom_has_rumble() const;

    bool get_mbc_ram_on () const;
    int get_mbc_rom_bank() const;
    int get_mbc_ram_bank() const;

    bool rom_is_mbc1  () const;
    bool get_mbc1_mode() const;

    bool is_cgb() const;

    bool is_activated() const;
    void set_activated(bool activated);

protected:
    virtual void print(const std::string &msg) = 0;

private:
    struct Command {
        CommandFunc func;
        int num_of_args;

        std::string command;
        std::string alias;

        std::string help_msg;
    };

    void add_command(CommandFunc func, const std::string &help_msg, int num_of_args, const std::string &command, const std::string &alias="");
    void tokenize(const std::string &line, std::vector <std::string> &tokens) const;

    // Command utilities
    int get_reg(const std::string &reg) const;
    int get_num(const std::string &num) const;
    int get_io(const std::string &io) const;
    int get_constant(const std::string &constant) const;

    void print_help_msg(const Command &command);

    // Commands
    void command_break(const std::vector <std::string> &tokens);
    void command_cartridge(const std::vector <std::string> &tokens);
    void command_continue(const std::vector <std::string> &tokens);
    void command_delete(const std::vector <std::string> &tokens);
    void command_disassemble(const std::vector <std::string> &tokens);
    void command_help(const std::vector <std::string> &tokens);
    void command_lcd(const std::vector <std::string> &tokens);
    void command_list(const std::vector <std::string> &tokens);
    void command_print(const std::vector <std::string> &tokens);
    void command_quit(const std::vector <std::string> &tokens);
    void command_registers(const std::vector <std::string> &tokens);
    void command_step(const std::vector <std::string> &tokens);
    void command_unwatch(const std::vector <std::string> &tokens);
    void command_watch(const std::vector <std::string> &tokens);

    GameBoy *gb;

    CpuDebugger cpu_debugger;
    MemoryDebugger memory_debugger;

    MemoryDisassembler disassembler;

    std::map <std::string, Command> commands;
    std::map <std::string, word> io_addrs;

    bool activated;
};
