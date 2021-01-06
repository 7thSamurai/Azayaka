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

#ifndef _SPRITE_HPP_
#define _SPRITE_HPP_

#include "types.hpp"

class State;

class Sprite
{
public:
    Sprite();

    byte read_byte(word address) const;
    void write_byte(word address, byte value);

    int get_y() const;
    int get_x() const;

    byte get_tile() const;

    bool priority    () const;
    bool y_flip      () const;
    bool x_flip      () const;
    bool dmg_palette () const;
    int color_vram   () const;
    int color_palette() const;

    void save_state(State &state);
    void load_state(State &state);

private:
    byte y, x;
    byte tile;
    byte flags;
};

#endif // _SPRITE_HPP_
