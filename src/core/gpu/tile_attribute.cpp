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

#include "core/gpu/tile_attribute.hpp"
#include "core/defs.hpp"
#include "core/state.hpp"

TileAttribute::TileAttribute() {
    value = 0;
}

byte TileAttribute::read_byte() const {
    return value;
}

void TileAttribute::write_byte(byte value) {
    this->value = value;
}

int TileAttribute::pal_num() const {
    return value & 0b111;
}

int TileAttribute::vbank() const {
    return (value & BIT3) >> 3;
}

bool TileAttribute::x_flip() const {
    return value & BIT5;
}

bool TileAttribute::y_flip() const {
    return value & BIT6;
}

bool TileAttribute::priority() const {
    return value & BIT7;
}

void TileAttribute::save_state(State &state) {
    state.write8(value);
}

void TileAttribute::load_state(State &state) {
    value = state.read8();
}
