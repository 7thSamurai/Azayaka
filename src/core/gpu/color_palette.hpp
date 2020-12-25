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

#ifndef _PALETTE_HPP_
#define _PALETTE_HPP_

#include "types.hpp"

class State;

class ColorPalette
{
public:
    ColorPalette();

    byte get_index() const;
    void set_index(byte index);

    byte get_palette() const;
    void set_palette(byte value);

    void save_state(State &state);
    void load_state(State &state);

    const Color *operator [] (int i) const;

private:
    byte index;
    bool auto_increment;

    Color palettes[8][4];
};

#endif // _PALETTE_HPP_
