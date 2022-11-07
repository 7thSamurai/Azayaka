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

#include "tester/suite.hpp"
#include "core/gameboy.hpp"
#include "core/cpu/cpu.hpp"
#include "core/memory/mmu.hpp"

const char *MooneyeSuite::name() const {
    return "mooneye";
}

const char *MooneyeSuite::title() const {
    return "Mooneye Test Suite";
}

bool MooneyeSuite::done(GameBoy &gb) {
    auto pc = gb.cpu->get_pc();

    // See https://github.com/Gekkio/mooneye-test-suite/#passfail-reporting
    if (gb.mmu->read_byte(pc+0) == 0x40 && // ld b, b
        gb.mmu->read_byte(pc+1) == 0x18 && // jr
        gb.mmu->read_byte(pc+2) == 0xFE)   // -2
        return true;

    return false;
}
