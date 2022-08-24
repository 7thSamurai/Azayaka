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

#include "core/gpu/sprite.hpp"
#include "core/defs.hpp"
#include "core/state.hpp"
#include "core/globals.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"

Sprite::Sprite() {
    y = x = 0x00;

    tile  = 0x00;
    flags = 0x00;
}

byte Sprite::read_byte(word address) const {
    switch (address & 0b11) {
        case 0: return y;
        case 1: return x;
        case 2: return tile;
        case 3: return flags;

        default:
            logger.log("Sprite::read_byte can't access address 0x" + hex(address, 4), Logger::Warning);
            break;
    }

    return 0;
}

void Sprite::write_byte(word address, byte value) {
    switch (address & 0b11) {
        case 0:
            y = value;
            break;

        case 1:
            x = value;
            break;

        case 2:
            tile = value;
            break;

        case 3:
            flags = value;
            break;

        default:
            logger.log("Sprite::write_byte can't 0x" + hex(address, 4), Logger::Warning);
            break;
    }
}

int Sprite::get_x() const {
    return x - 8;
}

int Sprite::get_y() const {
    return y - 16;
}

byte Sprite::get_tile() const {
    return tile;
}

bool Sprite::priority() const {
    return (flags & BIT7) != 0;
}

bool Sprite::y_flip() const {
    return (flags & BIT6) != 0;
}

bool Sprite::x_flip() const {
    return (flags & BIT5) != 0;
}

bool Sprite::dmg_palette() const {
    return (flags & BIT4) != 0;
}

int Sprite::color_vram() const {
    return (flags & BIT3) >> 3;
}

int Sprite::color_palette() const {
    return flags & 0b111;
}

void Sprite::save_state(State &state) {
    state.write8(y);
    state.write8(x);

    state.write8(tile);
    state.write8(flags);
}

void Sprite::load_state(State &state) {
    y = state.read8();
    x = state.read8();

    tile  = state.read8();
    flags = state.read8();
}
