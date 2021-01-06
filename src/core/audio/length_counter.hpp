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

#ifndef _LENGTH_COUNTER_HPP_
#define _LENGTH_COUNTER_HPP_

#include "types.hpp"

class State;

class LengthCounter
{
public:
    LengthCounter();

    void step();

    void set_nr4(byte value);

    bool is_enabled() const;
    bool is_zero() const;

    void set_length(byte length);
    void set_full_length(int full_length);

    void power_off(bool gbc_mode);

    void set_frame_sequencer(int frame_sequencer);

    void save_state(State &state);
    void load_state(State &state);

private:
    bool enabled;

    int full_length;
    int length;

    int frame_sequencer;
};

#endif // _LENGTH_COUNTER_HPP_
