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

#include "tester/suites/suite.hpp"
#include "core/gameboy.hpp"
#include "core/cpu/cpu.hpp"
#include "core/memory/mmu.hpp"

const char *BlarggSuite::name() const {
    return "blargg";
}

bool BlarggSuite::done(GameBoy &gb) {
    auto pc = gb.cpu->get_pc();

    if (gb.mmu->read_byte(pc+0) == 0x18 && // jp
        gb.mmu->read_byte(pc+1) == 0xFE)   // -2
        return true;

    if (gb.mmu->read_byte(pc+0) == 0xc3 &&        // jp
        gb.mmu->read_byte(pc+1) == (pc & 0xFF) && // LSB of pc
        gb.mmu->read_byte(pc+2) == (pc >> 8))     // MSB of pc
        return true;

    return false;
}
