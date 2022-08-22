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

#pragma once

#include "types.hpp"

class State;

class TileAttribute
{
public:
    TileAttribute();

    byte read_byte() const;
    void write_byte(byte value);

    int pal_num  () const; // Palette number
    int vbank    () const; // VRAM bank
    bool x_flip  () const; // Horizontal flip
    bool y_flip  () const; // Vertical flip
    bool priority() const; // BG to OAM priority

    void save_state(State &state);
    void load_state(State &state);

private:
    byte value;
};
