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

#include "color_palette.hpp"
#include "defs.hpp"
#include "state.hpp"

ColorPalette::ColorPalette() {
    index = 0;
    auto_increment = 0;

    for (int n = 0; n < 8; n++) {
        for (int i = 0; i < 4; i++)
            palettes[n][i] = Color{ 255, 255, 255, 255 };
    }
}

byte ColorPalette::get_index() const {
    return index | (auto_increment ? BIT7 : 0);
}

void ColorPalette::set_index(byte index) {
    this->index    = index & 0x3F;
    auto_increment = (index & BIT7) != 0;
}

byte ColorPalette::get_palette() const {
    byte i1 = index >> 3;
    byte i2 = (index & 7) >> 1;

    word color = ((palettes[i1][i2].r >> 3) << 0) |
                 ((palettes[i1][i2].g >> 3) << 5) |
                 ((palettes[i1][i2].b >> 3) << 10);

    if ((index & 1) == 0)
        return color & 0xFF;
    else
        return color >> 8;
}

void ColorPalette::set_palette(byte value) {
    byte i1 = index >> 3;
    byte i2 = (index & 7) >> 1;

    word color = ((palettes[i1][i2].r >> 3) << 0) |
                 ((palettes[i1][i2].g >> 3) << 5) |
                 ((palettes[i1][i2].b >> 3) << 10);

    if ((index & 1) == 0)
        color = (color & 0xFF00) | value;
    else
        color = (color & 0x00FF) | (value << 8);

    palettes[i1][i2].r = ((color >>  0) & 0x1F) << 3;
    palettes[i1][i2].g = ((color >>  5) & 0x1F) << 3;
    palettes[i1][i2].b = ((color >> 10) & 0x1F) << 3;

    if (auto_increment)
        index = (index + 1) & 0x3F;
}

void ColorPalette::save_state(State &state) {
    state.write8(index);
    state.write8(auto_increment);

    for (int i = 0; i < 8; i++)
        state.write_data(palettes[i], 4*sizeof(Color));
}

void ColorPalette::load_state(State &state) {
    index          = state.read8();
    auto_increment = state.read8();

    for (int i = 0; i < 8; i++)
        state.read_data(palettes[i], 4*sizeof(Color));
}

const Color *ColorPalette::operator [] (int i) const {
    return palettes[i];
}
