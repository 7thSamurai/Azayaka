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

#include "core/debug/memory_debugger.hpp"
#include "core/memory/mmu.hpp"
#include "common/string_utils.hpp"

#include <iostream>

MemoryDebugger::MemoryDebugger() {
}

MemoryDebugger::~MemoryDebugger() {
}

void MemoryDebugger::add_watchpoint(word address, Mmu *mmu) {
    watchpoints[address] = mmu->read_byte(address);
}

void MemoryDebugger::remove_watchpoint(word address) {
    auto it = watchpoints.find(address);

    if (it != watchpoints.end())
        watchpoints.erase(it);
}

void MemoryDebugger::clear_watchpoints() {
    watchpoints.clear();
}

void MemoryDebugger::get_watchpoints(std::vector <word> &watchpoints) {
    for (const auto &it : this->watchpoints)
        watchpoints.push_back(it.first);
}

bool MemoryDebugger::update_watchpoints(Mmu *mmu) {
    bool triggered = false;

    for (auto it = watchpoints.begin(); it != watchpoints.end(); it++) {
        byte data = mmu->read_byte(it->first);

        if (it->second != data) {
            it->second = data;
            triggered = true;
            std::cout << "Watchpoint triggered: 0x" << StringUtils::hex(it->first) << std::endl;
        }
    }

    return triggered;
}
