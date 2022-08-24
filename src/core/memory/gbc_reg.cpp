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

#include "core/memory/gbc_reg.hpp"
#include "core/gameboy.hpp"
#include "core/state.hpp"
#include "common/logger.hpp"
#include "common/string_utils.hpp"

GbcReg::GbcReg(GameBoy *gb) : Component(gb) {
    ff6c = ff72 = ff73 = ff74 = ff75 = 0x00;
}

byte GbcReg::read(word address) {
    switch (address) {
        case 0xFF6C:
            return gb->gbc_mode ? ff6c | 0xFE : 0xFF;

        case 0xFF72:
            return ff72;

        case 0xFF73:
            return ff73;

        case 0xFF74:
            return gb->gbc_mode ? ff74 : 0xFF;

        case 0xFF75:
            return ff75 | 0x8F;

        case 0xFF76:
        case 0xFF77:
            return 0x00;

        default:
            break;
    }

    LOG_WARNING("GbcReg::read can't handle address 0x" + StringUtils::hex(address));

    return 0;
}

void GbcReg::write(word address, byte value) {
    switch (address) {
        case 0xFF6C:
            ff6c = value & 1;
            return;

        case 0xFF72:
            ff72 = value;
            return;

        case 0xFF73:
            ff73 = value;
            return;

        case 0xFF74:
            ff74 = value;
            return;

        case 0xFF75:
            ff75 = value & 0x70;
            return;

        case 0xFF76:
        case 0xFF77:
            return;

        default:
            break;
    }

    LOG_WARNING("GbcReg::write can't handle address 0x" + StringUtils::hex(address));
}

void GbcReg::save_state(State &state) {
    state.write8(ff6c);
    state.write8(ff72);
    state.write8(ff73);
    state.write8(ff74);
    state.write8(ff75);
}

void GbcReg::load_state(State &state) {
    ff6c = state.read8();
    ff72 = state.read8();
    ff73 = state.read8();
    ff74 = state.read8();
    ff75 = state.read8();
}
