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

#include <vector>
#include <map>

class Mmu;

class MemoryDebugger
{
public:
    MemoryDebugger();
    ~MemoryDebugger();

    void add_watchpoint(word address, Mmu *mmu);
    void remove_watchpoint(word address);
    void clear_watchpoints();

    void get_watchpoints(std::vector <word> &watchpoints);

    // Returns true if any watch-points were triggered
    bool update_watchpoints(Mmu *mmu);

private:
    std::map <word, byte> watchpoints;
};
