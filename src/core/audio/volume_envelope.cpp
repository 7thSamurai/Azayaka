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

#include "volume_envelope.hpp"
#include "defs.hpp"
#include "state.hpp"

VolumeEnvelope::VolumeEnvelope() {
    power_off();
}

void VolumeEnvelope::step() {
    if (finished)
        return;

    if (--timer <= 0) {
        timer = (period != 0) ? period : 8;

        if (add_mode && volume < 15)
            volume++;
        else if (!add_mode && volume > 0)
            volume--;

        if (volume == 0 || volume == 15)
            finished = 1;
    }
}

void VolumeEnvelope::power_off() {
    finished = 1;
    timer = 0;

    starting_volume = 0;
    add_mode        = 0;
    period          = 0;

    volume = 0;
}

void VolumeEnvelope::set_nr2(byte value) {
    starting_volume = value >> 4;
    add_mode = (value & BIT3) != 0;
    period = value & 0b111;
}

byte VolumeEnvelope::get_nr2() const {
    return (starting_volume << 4) | (add_mode ? BIT3 : 0) | period;
}

byte VolumeEnvelope::get_volume() {
    if (period > 0)
        return volume;
    else
        return starting_volume;
}

void VolumeEnvelope::trigger() {
    volume = starting_volume;

    finished = 0;
    timer = (period != 0) ? period : 8;
}

void VolumeEnvelope::save_state(State &state) {
    state.write8(finished);
    state.write32(timer);

    state.write8(starting_volume);
    state.write8(add_mode);
    state.write8(period);

    state.write8(volume);
}

void VolumeEnvelope::load_state(State &state) {
    finished = state.read8();
    timer    = state.read32();

    starting_volume = state.read8();
    add_mode = state.read8();
    period   = state.read8();

    volume = state.read8();
}
