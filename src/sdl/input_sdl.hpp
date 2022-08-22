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

#include "input/input.hpp"

class InputSDL : public Input
{
public:
    InputSDL();

    std::string get_key_name(ButtonType button) const override;
    void map_key(ButtonType button, const std::string &name) override;

    void update() override;

    void reset();

private:
    struct Button {
        int scancode;
        bool state;
    };

    std::string get_name_from_scancode(int scancode) const;
    int get_scancode_from_name(const std::string &name) const;

    Button buttons[num_of_buttons];
};
