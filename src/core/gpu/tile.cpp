// Copyright (C) 2020 Zach Collins
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

#include "tile.hpp"
#include "state.hpp"

#include <algorithm>

Tile::Tile() {
    std::fill(data, data+16, 0);
}

byte Tile::read_byte(word address) const {
    return data[address & 0b1111];
}

void Tile::write_byte(word address, byte value) {
    data[address & 0b1111] = value;
}

int Tile::get_pixel(int x, int y) const {
    bool lo = data[(y << 1)+0] & (1 << (7-x));
    bool hi = data[(y << 1)+1] & (1 << (7-x));

    return (lo ? 1 : 0) | (hi ? 2 : 0);
}

void Tile::save_state(State &state) {
    state.write_data(data, 16);
}

void Tile::load_state(State &state) {
    state.read_data(data, 16);
}
