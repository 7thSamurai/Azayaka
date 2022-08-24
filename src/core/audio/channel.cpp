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

#include "core/audio/channel.hpp"
#include "core/defs.hpp"
#include "core/state.hpp"

Channel::Channel(GameBoy *gb) : Component(gb) {
    channel_enabled = 0;
    dac_enabled     = 0;

    output = 0;
}

byte Channel::get_output() const {
    return output;
}

bool Channel::is_enabled() const {
    return channel_enabled && dac_enabled;
}

void Channel::length_clock() {
    length_counter.step();

    if (length_counter.is_enabled() && length_counter.is_zero())
        channel_enabled = 0;
}

void Channel::sweep_clock() {
}

void Channel::envelope_clock() {
}

void Channel::set_frame_sequencer(int frame_sequencer) {
    length_counter.set_frame_sequencer(frame_sequencer);
}

void Channel::save_state(State &state) {
    length_counter.save_state(state);

    state.write8(channel_enabled);
    state.write8(dac_enabled);

    state.write8(output);
}

void Channel::load_state(State &state) {
    length_counter.load_state(state);

    channel_enabled = state.read8();
    dac_enabled     = state.read8();

    output = state.read8();
}
