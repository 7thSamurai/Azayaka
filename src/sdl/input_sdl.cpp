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

#include "sdl/input_sdl.hpp"
#include "core/input/joypad.hpp"

#include <map>
#include <SDL_keyboard.h>

static const std::map <SDL_Keycode, std::string> keycodes = {
    {SDL_SCANCODE_RETURN, "Return"},
    {SDL_SCANCODE_ESCAPE, "Escape"},
    {SDL_SCANCODE_BACKSPACE, "Backspace"},
    {SDL_SCANCODE_TAB, "Tab"},
    {SDL_SCANCODE_SPACE, "Space"},
    {SDL_SCANCODE_0, "0"},
    {SDL_SCANCODE_1, "1"},
    {SDL_SCANCODE_2, "2"},
    {SDL_SCANCODE_3, "3"},
    {SDL_SCANCODE_4, "4"},
    {SDL_SCANCODE_5, "5"},
    {SDL_SCANCODE_6, "6"},
    {SDL_SCANCODE_7, "7"},
    {SDL_SCANCODE_8, "8"},
    {SDL_SCANCODE_9, "9"},
    {SDL_SCANCODE_A, "A"},
    {SDL_SCANCODE_B, "B"},
    {SDL_SCANCODE_C, "C"},
    {SDL_SCANCODE_D, "D"},
    {SDL_SCANCODE_E, "E"},
    {SDL_SCANCODE_F, "F"},
    {SDL_SCANCODE_G, "G"},
    {SDL_SCANCODE_H, "H"},
    {SDL_SCANCODE_I, "I"},
    {SDL_SCANCODE_J, "J"},
    {SDL_SCANCODE_K, "K"},
    {SDL_SCANCODE_L, "L"},
    {SDL_SCANCODE_M, "M"},
    {SDL_SCANCODE_N, "N"},
    {SDL_SCANCODE_O, "O"},
    {SDL_SCANCODE_P, "P"},
    {SDL_SCANCODE_Q, "Q"},
    {SDL_SCANCODE_R, "R"},
    {SDL_SCANCODE_S, "S"},
    {SDL_SCANCODE_T, "T"},
    {SDL_SCANCODE_U, "U"},
    {SDL_SCANCODE_V, "V"},
    {SDL_SCANCODE_W, "W"},
    {SDL_SCANCODE_X, "X"},
    {SDL_SCANCODE_Y, "Y"},
    {SDL_SCANCODE_Z, "Z"},
    {SDL_SCANCODE_LCTRL, "Left Ctrl"},
    {SDL_SCANCODE_RCTRL, "Right Ctrl"},
    {SDL_SCANCODE_LSHIFT, "Left Shift"},
    {SDL_SCANCODE_RSHIFT, "Right Shift"},
    {SDL_SCANCODE_LALT, "Left Alt"},
    {SDL_SCANCODE_RALT, "Right Alt"},
    {SDL_SCANCODE_RIGHT, "Right"},
    {SDL_SCANCODE_LEFT, "Left"},
    {SDL_SCANCODE_DOWN, "Down"},
    {SDL_SCANCODE_UP, "Up"},
    {SDL_SCANCODE_KP_0, "Keypad 0"},
    {SDL_SCANCODE_KP_1, "Keypad 1"},
    {SDL_SCANCODE_KP_2, "Keypad Down"},
    {SDL_SCANCODE_KP_3, "Keypad 3"},
    {SDL_SCANCODE_KP_4, "Keypad Left"},
    {SDL_SCANCODE_KP_5, "Keypad 5"},
    {SDL_SCANCODE_KP_6, "Keypad Right"},
    {SDL_SCANCODE_KP_7, "Keypad 7"},
    {SDL_SCANCODE_KP_8, "Keypad Up"},
    {SDL_SCANCODE_KP_ENTER, "Keypad Enter"}
};

InputSDL::InputSDL() {
    for (Button &b : buttons)
        b.state = 0;
}

std::string InputSDL::get_key_name(ButtonType button) const {
    return get_name_from_scancode(buttons[button].scancode);
}

void InputSDL::map_key(ButtonType button, const std::string &name) {
    buttons[button].scancode = get_scancode_from_name(name);
}

void InputSDL::update() {
    const Uint8 *states = SDL_GetKeyboardState(NULL);

    for (int i = 0; i < num_of_buttons; i++) {
        bool state = states[buttons[i].scancode];

        if (state != buttons[i].state)
            joypad->on_key_event((ButtonType)i, state);

        buttons[i].state = state;
    }
}

void InputSDL::reset() {
    for (int i = 0; i < 8; i++) {
        buttons[i].state = 0;
        joypad->on_key_event((ButtonType)i, 0);
    }
}

std::string InputSDL::get_name_from_scancode(int scancode) const {
    auto it = keycodes.find(scancode);

    return it != keycodes.end() ? it->second : "";
}

int InputSDL::get_scancode_from_name(const std::string &name) const {
    for (auto &it : keycodes) {
        if (it.second == name)
            return it.first;
    }

    return -1;
}
