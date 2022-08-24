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

#include "core/audio/channel2.hpp"
#include "core/gameboy.hpp"
#include "core/defs.hpp"
#include "core/state.hpp"
#include "common/logger.hpp"
#include "common/string_utils.hpp"

Channel2::Channel2(GameBoy *gb) : Channel(gb) {
    timer     = 0;
    sequence  = 0;
    frequency = 0;

    duty = 0;

    length_counter.set_full_length(64);
}

byte Channel2::read(word address) {
    switch (address) {
        case 0xFF15:
            return 0xFF;

        case 0xFF16:
            return (duty << 6) | 0x3F;

        case 0xFF17:
            return volume_envelope.get_nr2();

        case 0xFF18:
            return 0xFF;

        case 0xFF19:
            return (length_counter.is_enabled() ? BIT6 : 0) | 0xBF;

        default:
            break;
    }

    LOG_WARNING("Channel2::read can't access address 0x" + hex(address));

    return 0;
}

void Channel2::write(word address, byte value) {
    switch (address) {
        case 0xFF15:
            return;

        case 0xFF16:
            duty = value >> 6;
            length_counter.set_length(value & 0x3F);
            return;

        case 0xFF17:
            dac_enabled = (value & 0xF8) != 0;
            channel_enabled &= dac_enabled;

            volume_envelope.set_nr2(value);
            return;

        case 0xFF18:
            frequency = (frequency & 0x700) | value;
            return;

        case 0xFF19:
            frequency = (frequency & 0xFF) | ((value & 0b111) << 8);
            length_counter.set_nr4(value);

            if (length_counter.is_enabled() && length_counter.is_zero())
                channel_enabled = 0;
            else if (value & BIT7)
                trigger();
            return;

        default:
            break;
    }

    LOG_WARNING("Channel2::write can't access address 0x" + hex(address));
}

void Channel2::tick() {
    if (--timer <= 0) {
        timer = (2048 - frequency) << 2;

        sequence = (sequence + 1) & 7;

        if (is_enabled())
            output = duty_table[duty][sequence] ? volume_envelope.get_volume() : 0;
        else
            output = 0;
    }
}

void Channel2::envelope_clock() {
    volume_envelope.step();
}

void Channel2::power_off() {
    volume_envelope.power_off();
    length_counter .power_off(gb->gbc_mode);

    channel_enabled = 0;
    dac_enabled     = 0;

    sequence  = 0;
    frequency = 0;

    duty = 0;
}

void Channel2::trigger() {
    timer = (2048 - frequency) << 2;

    volume_envelope.trigger();

    channel_enabled = dac_enabled;
}

void Channel2::save_state(State &state) {
    Channel::save_state(state);

    volume_envelope.save_state(state);

    state.write32(timer);
    state.write32(sequence);
    state.write32(frequency);

    state.write8(duty);
}

void Channel2::load_state(State &state) {
    Channel::load_state(state);

    volume_envelope.load_state(state);

    timer     = state.read32();
    sequence  = state.read32();
    frequency = state.read32();

    duty = state.read8();
}
