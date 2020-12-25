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

#include "input.hpp"
#include "settings.hpp"

void Input::bind_joypad(Joypad *joypad) {
    this->joypad = joypad;
}

void Input::load_settings(Settings &settings) {
    map_key(ButtonType_A,      settings.input_a);
    map_key(ButtonType_B,      settings.input_b);
    map_key(ButtonType_Up,     settings.input_up);
    map_key(ButtonType_Down,   settings.input_down);
    map_key(ButtonType_Left,   settings.input_left);
    map_key(ButtonType_Right,  settings.input_right);
    map_key(ButtonType_Start,  settings.input_start);
    map_key(ButtonType_Select, settings.input_select);
}
