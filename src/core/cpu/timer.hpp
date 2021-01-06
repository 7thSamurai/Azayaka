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

#ifndef _TIMER_HPP_
#define _TIMER_HPP_

#include "component.hpp"
#include "types.hpp"

class State;

class Timer : public Component
{
public:
    Timer(GameBoy *gb);

    byte read(word address) override;
    void write(word address, byte value) override;

    void tick();

    void save_state(State &state);
    void load_state(State &state);

private:
    void tima_glitch(bool old_enabled, word old_bit);

    word div;  // Divider Register
    byte tima; // Timer counter
    byte tma;  // Timer Modulo
    byte tac;  // Timer Control

    const byte bits[4] = { 9, 3, 5, 7 };

    bool timer_enabled; // Is the timer enabled
    word current_bit;
    bool last_bit;

    bool overflow;
    int ticks_since_overflow;
};

#endif // _TIMER_HPP_
