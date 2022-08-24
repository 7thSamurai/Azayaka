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

#include "core/types.hpp"

#include <string>
#include <vector>

class Mmu;

class Disassembler
{
public:
    Disassembler();
    virtual ~Disassembler();

protected:
    virtual byte read_byte() = 0;
    word read_word();

    std::string read_branch();
    std::string read_in(std::string &comment);
    std::string read_inn(std::string &comment);
    std::string read_io(word address);

    // Returns the size of the opcode
    int disassemble_instr(byte op_code, std::string &instr);

    unsigned int pc;
    int size;
};

class FileDisassembler : public Disassembler
{
public:
    int disassemble(const std::string &file_path);
    int disassemble(const std::string &file_path, const std::string &rom_usage_path);

protected:
    byte read_byte();

private:
    void print_data(const std::vector <byte> &data_bytes);

    byte *data;
};

class MemoryDisassembler : public Disassembler
{
public:
    // Returns the size of the opcode
    int disassemble_byte(Mmu *mmu, word address, std::string &instr);

protected:
    byte read_byte();

private:
    Mmu *mmu;
};
