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

#include "core/audio/channel4.hpp"
#include "core/gameboy.hpp"
#include "core/defs.hpp"
#include "core/state.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"

Channel4::Channel4(GameBoy *gb) : Channel(gb) {
    timer = 0;

    clock_shift  = 0;
    width_mode   = 0;
    divisor_code = 0;

    lfsr = 0x7fff;

    length_counter.set_full_length(64);
}

byte Channel4::read(word address) {
    switch (address) {
        case 0xFF1F:
            return 0xFF;

        case 0xFF20:
            return 0xFF;

        case 0xFF21:
            return volume_envelope.get_nr2();

        case 0xFF22:
            return (clock_shift << 4) | (width_mode ? BIT3 : 0) | divisor_code;

        case 0xFF23:
            return (length_counter.is_enabled() ? BIT6 : 0) | 0xBF;

        default:
            break;
    }

    LOG_WARNING("Channel4::read can't access address 0x" + hex(address, 4));

    return 0;
}

void Channel4::write(word address, byte value) {
    switch (address) {
        case 0xFF1F:
            return;

        case 0xFF20:
            length_counter.set_length(value & 0x3F);
            return;

        case 0xFF21:
            dac_enabled = (value & 0xF8) != 0;
            channel_enabled &= dac_enabled;

            volume_envelope.set_nr2(value);
            return;

        case 0xFF22:
            clock_shift  = value >> 4;
            width_mode   = (value & BIT3) != 0;
            divisor_code = value & 0b111;
            return;

        case 0xFF23:
            length_counter.set_nr4(value);

            if (length_counter.is_enabled() && length_counter.is_zero())
                channel_enabled = 0;
            else if (value & BIT7)
                trigger();
            return;

        default:
            break;
    }

    LOG_WARNING("Channel4::write can't access address 0x" + hex(address, 4));
}

void Channel4::tick() {
    if (--timer <= 0) {
        timer = divisors[divisor_code] << clock_shift;

        bool result = ((lfsr & 1) ^ ((lfsr >> 1) & 1)) != 0;
        lfsr >>= 1;
        lfsr |= result ? (1 << 14) : 0;

        if (width_mode) {
            lfsr &= ~BIT6;
            lfsr |= result ? BIT6 : 0;
        }

        if (is_enabled() && (lfsr & 1) == 0)
            output = volume_envelope.get_volume();
        else
            output = 0;
    }
}

void Channel4::envelope_clock() {
    volume_envelope.step();
}

void Channel4::power_off() {
    volume_envelope.power_off();
    length_counter .power_off(gb->gbc_mode);

    channel_enabled = 0;
    dac_enabled     = 0;

    clock_shift  = 0;
    width_mode   = 0;
    divisor_code = 0;
}

void Channel4::trigger() {
    volume_envelope.trigger();

    timer = divisors[divisor_code] << clock_shift;

    lfsr = 0x7fff;

    channel_enabled = dac_enabled;
}

void Channel4::save_state(State &state) {
    Channel::save_state(state);
    volume_envelope.save_state(state);

    state.write32(timer);
    state.write32(clock_shift);
    state.write8(width_mode);
    state.write32(divisor_code);

    state.write32(lfsr);
}

void Channel4::load_state(State &state) {
    Channel::load_state(state);
    volume_envelope.load_state(state);

    timer        = state.read32();
    clock_shift  = state.read32();
    width_mode   = state.read8();
    divisor_code = state.read32();

    lfsr = state.read32();
}
