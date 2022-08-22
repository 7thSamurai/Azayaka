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

#include "component.hpp"
#include "length_counter.hpp"

class State;

class Channel : public Component
{
public:
    Channel(GameBoy *gb);

    virtual ~Channel() { }

    virtual void tick() = 0;
    virtual void power_off() = 0;

    byte get_output() const;
    bool is_enabled() const;

    void length_clock();
    virtual void sweep_clock();
    virtual void envelope_clock();

    void set_frame_sequencer(int frame_sequencer);

    virtual void save_state(State &state);
    virtual void load_state(State &state);

protected:
    LengthCounter length_counter;

    bool channel_enabled;
    bool dac_enabled;

    byte output;
};
