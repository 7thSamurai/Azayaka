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

#include "frequency_sweep.hpp"
#include "defs.hpp"
#include "state.hpp"

#include <iostream>

FrequencySweep::FrequencySweep() {
    power_off();
}

void FrequencySweep::step() {
    if (!enabled)
        return;

    if (--timer <= 0) {
        timer = (period != 0) ? period : 8;

        if (period != 0) {
            int new_frequency = calculate();

            if (overflow == 0 && shift != 0) {
                shadow_frequency = new_frequency;
                frequency        = new_frequency;

                calculate();
            }
        }
    }
}

bool FrequencySweep::is_enabled() const {
    return overflow == 0;
}

void FrequencySweep::power_off() {
    enabled     = 0;
    overflow    = 0;
    has_negated = 0;

    timer = 0;

    frequency        = 0;
    shadow_frequency = 0;

    period = 0;
    negate = 0;
    shift  = 0;
}

void FrequencySweep::set_nr10(byte value) {
    period = (value >> 4) & 0b111;
    negate = (value & BIT3) != 0;
    shift  = value & 0b111;

    // Disable
    if (has_negated && !negate)
        overflow = 1;
}

void FrequencySweep::set_nr13(byte value) {
    frequency = (frequency & 0x700) | value;
}

void FrequencySweep::set_nr14(byte value) {
    frequency = (frequency & 0xFF) | ((value & 0b111) << 8);
}

byte FrequencySweep::get_nr10() const {
    return (period << 4) | (negate ? BIT3 : 0) | shift;
}

int FrequencySweep::get_frequency() {
    return frequency;
}

void FrequencySweep::trigger() {
    overflow    = 0;
    has_negated = 0;

    shadow_frequency = frequency;

    timer  = (period != 0) ? period : 8;
    enabled = period != 0 || shift != 0;

    if (shift > 0)
        calculate();
}

int FrequencySweep::calculate() {
    int new_freq = shadow_frequency >> shift;

    if (negate) {
        new_freq = shadow_frequency - new_freq;
        has_negated = 1;
    }
    else
        new_freq = shadow_frequency + new_freq;

    if (new_freq > 2047)
        overflow = 1;

    return new_freq;
}

void FrequencySweep::save_state(State &state) {
    state.write8(enabled);
    state.write8(overflow);
    state.write8(has_negated);

    state.write32(timer);
    state.write32(frequency);
    state.write32(shadow_frequency);

    state.write8(period);
    state.write8(negate);
    state.write8(shift);
}

void FrequencySweep::load_state(State &state) {
    enabled     = state.read8();
    overflow    = state.read8();
    has_negated = state.read8();

    timer            = state.read32();
    frequency        = state.read32();
    shadow_frequency = state.read32();

    period = state.read8();
    negate = state.read8();
    shift  = state.read8();
}
