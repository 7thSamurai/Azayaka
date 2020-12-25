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

#include "channel3.hpp"
#include "gameboy.hpp"
#include "defs.hpp"
#include "globals.hpp"
#include "state.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"

Channel3::Channel3(GameBoy *gb) : Channel(gb) {
    timer    = 0;
    position = 4;
    ticks_since_read = 0;

    frequency    = 0;
    volume_code  = 0;
    last_address = 0;

    length_counter.set_full_length(256);
}

byte Channel3::read(word address) {
    if (address >= 0xFF30 && address <= 0xFF3F) {
        if (is_enabled()) {
            if (ticks_since_read < 2 || gb->gbc_mode)
                return wave_table[last_address];
            else
                return 0xFF;
        }
        else
            return wave_table[address - 0xFF30];
    }

    switch (address) {
        case 0xFF1A:
            return (dac_enabled ? BIT7 : 0) | 0x7F;

        case 0xFF1B:
            return 0xFF;

        case 0xFF1C:
            return (volume_code << 5) | 0x9F;

        case 0xFF1D:
            return 0xFF;

        case 0xFF1E:
            return (length_counter.is_enabled() ? BIT6 : 0) | 0xBF;

        default:
            break;
    }

    logger.log("Channel3::read can't access address 0x" + hex(address, 4), Logger::Warning);

    return 0;
}

void Channel3::write(word address, byte value) {
    if (address >= 0xFF30 && address <= 0xFF3F) {
        if (is_enabled()) {
            if (ticks_since_read < 2 || gb->gbc_mode)
                wave_table[last_address] = value;
        }
        else
            wave_table[address - 0xFF30] = value;
        return;
    }

    switch (address) {
        case 0xFF1A:
            dac_enabled = (value & BIT7) != 0;
            channel_enabled &= dac_enabled;
            return;

        case 0xFF1B:
            length_counter.set_length(value);
            return;

        case 0xFF1C:
            volume_code = (value >> 5) & 0b11;
            return;

        case 0xFF1D:
            frequency = (frequency & 0x700) | value;
            return;

        case 0xFF1E:
            length_counter.set_nr4(value);
            frequency = (frequency & 0xFF) | ((value & 0b111) << 8);

            if (length_counter.is_enabled() && length_counter.is_zero())
                channel_enabled = 0;
            else if (value & BIT7)
                trigger();
            return;

        default:
            break;
    }

    logger.log("Channel3::write can't access address 0x" + hex(address, 4), Logger::Warning);
}

void Channel3::tick() {
    ticks_since_read++;

    if (--timer <= 0) {
        timer = (2048 - frequency) << 1;

        if (is_enabled()) {
            ticks_since_read = 0;

            last_address = position >> 1;
            output = wave_table[last_address];

            if (position & 1)
                output &= 0x0F;
            else
                output >>= 4;

            if (volume_code > 0)
                output >>= (volume_code - 1);
            else
                output = 0;

            position = (position + 1) & 31;
        }
        else
            output = 0;
    }
}

void Channel3::power_off() {
    length_counter.power_off(gb->gbc_mode);

    channel_enabled = 0;
    dac_enabled     = 0;

    position    = 0;
    frequency   = 0;
    volume_code = 0;

    ticks_since_read = 0;
    last_address     = 0;
}

void Channel3::trigger() {
    // Triggering while it reads a sample byte corrupts the data
    if (is_enabled() && timer == 2 && !gb->gbc_mode) {
        int pos = position >> 1;

        if (pos < 4)
            wave_table[0] = wave_table[pos];
        else {
            pos &= ~0b11; // Make it 4-byte aligned

            std::copy(&wave_table[pos], &wave_table[pos+4], &wave_table[0]);
        }
    }

    timer = 6; // Note: You need this to get blargg's "09-wave read while on" test

    position = 0;
    last_address = 0;

    channel_enabled = dac_enabled;
}

void Channel3::save_state(State &state) {
    Channel::save_state(state);

    state.write_data(wave_table, 16);

    state.write32(timer);
    state.write32(position);
    state.write32(ticks_since_read);
    state.write32(last_address);

    state.write32(frequency);
    state.write8(volume_code);
}

void Channel3::load_state(State &state) {
    Channel::load_state(state);

    state.read_data(wave_table, 16);

    timer    = state.read32();
    position = state.read32();
    ticks_since_read = state.read32();
    last_address     = state.read32();

    frequency   = state.read32();
    volume_code = state.read8();
}
