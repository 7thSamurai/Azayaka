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

#include "core/input/joypad.hpp"
#include "core/gameboy.hpp"
#include "core/state.hpp"
#include "core/cpu/cpu.hpp"
#include "core/defs.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"

Joypad::Joypad(GameBoy *gb) : Component(gb) {
    reset();
}

void Joypad::reset() {
    column = 0;

    keys[0] = keys[1] = 0x0F;
}

void Joypad::on_key_event(Input::ButtonType type, bool state) {
    if (state) {
        bool b_hit = 0;
        bool d_hit = 0;

        switch (type) {
            case Input::ButtonType_A:
                keys[0] &= ~BIT0;
                b_hit = 1;
                break;
            case Input::ButtonType_B:
                keys[0] &= ~BIT1;
                b_hit = 1;
                break;
            case Input::ButtonType_Select:
                b_hit = 1;
                keys[0] &= ~BIT2;
                break;
            case Input::ButtonType_Start:
                b_hit = 1;
                keys[0] &= ~BIT3;
                break;

            case Input::ButtonType_Right:
                d_hit = 1;
                keys[1] &= ~BIT0;
                break;
            case Input::ButtonType_Left:
                d_hit = 1;
                keys[1] &= ~BIT1;
                break;
            case Input::ButtonType_Up:
                d_hit = 1;
                keys[1] &= ~BIT2;
                break;
            case Input::ButtonType_Down:
                d_hit = 1;
                keys[1] &= ~BIT3;
                break;

            default:
                return;
        }

        if ((d_hit && column == 0x10) || (b_hit && column == 0x20))
            gb->cpu->trigger_interrupt(INT60);
    }

    else {
        switch (type) {
            case Input::ButtonType_A:
                keys[0] |= BIT0;
                break;
            case Input::ButtonType_B:
                keys[0] |= BIT1;
                break;
            case Input::ButtonType_Select:
                keys[0] |= BIT2;
                break;
            case Input::ButtonType_Start:
                keys[0] |= BIT3;
                break;

            case Input::ButtonType_Right:
                keys[1] |= BIT0;
                break;
            case Input::ButtonType_Left:
                keys[1] |= BIT1;
                break;
            case Input::ButtonType_Up:
                keys[1] |= BIT2;
                break;
            case Input::ButtonType_Down:
                keys[1] |= BIT3;
                break;

            default:
                return;
        }
    }
}

byte Joypad::read(word address) {
    if (address == 0xFF00) {
        if (column == 0x10)
            return keys[0] | 0xC0;
        else if (column == 0x20)
            return keys[1] | 0xC0;
        else
            return 0xCF;
    }

    LOG_WARNING("Joypad::read can't access address 0x" + hex(address));

    return 0;
}

void Joypad::write(word address, byte value) {
    if (address == 0xFF00)
        column = value & 0x30;

    else
        LOG_WARNING("Joypad::write can't access address 0x" + hex(address));
}

void Joypad::save_state(State &state) {
    state.write8(keys[0]);
    state.write8(keys[1]);

    state.write8(column);
}

void Joypad::load_state(State &state) {
    keys[0] = state.read8();
    keys[1] = state.read8();

    column = state.read8();
}
