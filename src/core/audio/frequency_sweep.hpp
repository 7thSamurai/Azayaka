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

#ifndef _FREQUENCY_SWEEP_HPP_
#define _FREQUENCY_SWEEP_HPP_

#include "types.hpp"

class State;

class FrequencySweep
{
public:
    FrequencySweep();

    void step();

    bool is_enabled() const;

    void power_off();

    void set_nr10(byte value);
    void set_nr13(byte value);
    void set_nr14(byte value);

    byte get_nr10() const;

    int get_frequency();

    void trigger();

    void save_state(State &state);
    void load_state(State &state);

private:
    int calculate();

    bool enabled;
    bool overflow;
    bool has_negated;

    int timer;

    int frequency;
    int shadow_frequency;

    byte period;
    bool negate;
    byte shift;
};

#endif // _FREQUENCY_SWEEP_HPP_
