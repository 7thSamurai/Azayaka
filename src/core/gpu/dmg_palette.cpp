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

#include "dmg_palette.hpp"
#include "state.hpp"

DmgPalette::DmgPalette() {
    palette = 0x00;

    for (int i = 0; i < 4; i++)
        colors[i] = Color{ 0x00, 0x00, 0x00, 0xFF };
}

byte DmgPalette::get_palette() const {
    return palette;
}

void DmgPalette::set_palette(byte palette) {
    this->palette = palette;

    for (int i = 0; i < 4; i++, palette >>= 2) {
        switch (palette & 0b11) {
            case 0: // White
                colors[i] = { 0xFF, 0xFF, 0xFF, 0xFF };
                break;
            case 1: // Light grey
                colors[i] = { 0xC0, 0xC0, 0xC0, 0xFF };
                break;
            case 2: // Dark grey
                colors[i] = { 0x60, 0x60, 0x60, 0xFF };
                break;
            case 3: // Black
                colors[i] = { 0x00, 0x00, 0x00, 0xFF };
        }
    }
}

const Color DmgPalette::operator [] (int i) const {
    return colors[i];
}

void DmgPalette::save_state(State &state) {
    state.write8(palette);
}

void DmgPalette::load_state(State &state) {
    set_palette(state.read8());
}
