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

#include "cpu_debugger.hpp"
#include "cpu/cpu.hpp"

CpuDebugger::CpuDebugger() {
}

CpuDebugger::~CpuDebugger() {
}

void CpuDebugger::add_breakpoint(word address) {
    breakpoints.push_back(address);
}

void CpuDebugger::remove_breakpoint(word address) {
    int i = find_breakpoint(address);

    if (i != -1)
        breakpoints.erase(breakpoints.begin() + i);
}

void CpuDebugger::clear_breakpoints() {
    breakpoints.clear();
}

void CpuDebugger::get_breakpoints(std::vector <word> &breakpoints) {
    for (word address : this->breakpoints)
        breakpoints.push_back(address);
}

int CpuDebugger::find_breakpoint(word address) {
    for (unsigned int i = 0; i < breakpoints.size(); i++) {
        if (address == breakpoints.at(i))
            return i;
    }

    return -1;
}

void CpuDebugger::step(Cpu *cpu) {
    cpu->step();
}

int CpuDebugger::check_pc(Cpu *cpu) {
    for (word address : breakpoints) {
        if (address == cpu->get_pc())
            return address;
    }

    return -1;
}
