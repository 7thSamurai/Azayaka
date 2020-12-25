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

#include "length_counter.hpp"
#include "defs.hpp"
#include "state.hpp"

LengthCounter::LengthCounter() {
    enabled = 0;
    length  = 0;

    full_length = 0;

    frame_sequencer = 0;
}

void LengthCounter::step() {
    if (enabled && length > 0)
        length--;
}

void LengthCounter::set_nr4(byte value) {
    bool enable  = (value & BIT6) != 0;
    bool trigger = (value & BIT7) != 0;

    if (enabled) {
        if (trigger && length == 0) {
            if (enable && frame_sequencer & 1)
                length = full_length - 1;
            else
                length = full_length;
        }
    }

    else if (enable) {
        if (frame_sequencer & 1) {
            if (length != 0)
                length--; // Clock it

            if (trigger && length == 0)
                length = full_length - 1; // Clock it
        }
    }

    else {
        if (trigger && length == 0)
            length = full_length;
    }

    enabled = enable;
}

bool LengthCounter::is_enabled() const {
    return enabled;
}

bool LengthCounter::is_zero() const {
    return length == 0;
}

void LengthCounter::set_length(byte length) {
    if (length == 0)
        this->length = full_length;
    else
        this->length = full_length - length;
}

void LengthCounter::set_full_length(int full_length) {
    this->full_length = full_length;
}

void LengthCounter::power_off(bool gbc_mode) {
    enabled = 0;
    frame_sequencer = 0;

    if (gbc_mode)
        length = full_length;
}

void LengthCounter::set_frame_sequencer(int frame_sequencer) {
    this->frame_sequencer = frame_sequencer;
}

void LengthCounter::save_state(State &state) {
    state.write8(enabled);

    state.write32(full_length);
    state.write32(length);

    state.write32(frame_sequencer);
}

void LengthCounter::load_state(State &state) {
    enabled = state.read8();

    full_length = state.read32();
    length      = state.read32();

    frame_sequencer = state.read32();
}
