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

#include "types.hpp"

#include <vector>

class Cpu;

class CpuDebugger
{
public:
    CpuDebugger();
    ~CpuDebugger();

    void add_breakpoint(word address);
    void remove_breakpoint(word address);
    void clear_breakpoints();

    void get_breakpoints(std::vector <word> &breakpoints);

    void step(Cpu *cpu);

    // Returns -1 if no breakpoint was reached, else the breakpoint
    int check_pc(Cpu *cpu);

private:
    int find_breakpoint(word address);

    std::vector <word> breakpoints;
};
