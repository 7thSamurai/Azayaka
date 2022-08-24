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

#include "core/audio/apu.hpp"
#include "core/gameboy.hpp"
#include "core/defs.hpp"
#include "core/state.hpp"
#include "core/settings.hpp"
#include "core/audio/audio_driver.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"

#include "core/audio/channel1.hpp"
#include "core/audio/channel2.hpp"
#include "core/audio/channel3.hpp"
#include "core/audio/channel4.hpp"

#include <iostream>

Apu::Apu(GameBoy *gb) : Component(gb) {
    audio_driver = nullptr;

    channels[0] = new Channel1(gb);
    channels[1] = new Channel2(gb);
    channels[2] = new Channel3(gb);
    channels[3] = new Channel4(gb);

    vin_left_enable = vin_right_enable = 0;
    left_volume = right_volume = 0;

    enabled = 0;

    for (int i = 0; i < 4; i++) {
        left_enables [i] = 0;
        right_enables[i] = 0;
        volume[i] = 1.0f;
    }

    frequency_counter = 95;

    frame_sequencer_counter = 8192;
    frame_sequencer = 0;
}

Apu::~Apu() {
    for (int i = 0; i < 4; i++)
        delete channels[i];
}

void Apu::bind_audio_driver(AudioDriver *audio_driver) {
    this->audio_driver = audio_driver;
}

void Apu::tick() {
    if (--frame_sequencer_counter <= 0) {
        frame_sequencer_counter = 8192;

        switch (frame_sequencer) {
            case 0:
                channels[0]->length_clock();
                channels[1]->length_clock();
                channels[2]->length_clock();
                channels[3]->length_clock();
                break;

            case 1:
                break;

            case 2:
                channels[0]->sweep_clock ();
                channels[0]->length_clock();
                channels[1]->length_clock();
                channels[2]->length_clock();
                channels[3]->length_clock();
                break;

            case 3:
                break;

            case 4:
                channels[0]->length_clock();
                channels[1]->length_clock();
                channels[2]->length_clock();
                channels[3]->length_clock();
                break;

            case 5:
                break;

            case 6:
                channels[0]->sweep_clock ();
                channels[0]->length_clock();
                channels[1]->length_clock();
                channels[2]->length_clock();
                channels[3]->length_clock();
                break;

            case 7:
                channels[0]->envelope_clock();
                channels[1]->envelope_clock();
                channels[3]->envelope_clock();
                break;
        }

        frame_sequencer = (frame_sequencer + 1) & 7;

        channels[0]->set_frame_sequencer(frame_sequencer);
        channels[1]->set_frame_sequencer(frame_sequencer);
        channels[2]->set_frame_sequencer(frame_sequencer);
        channels[3]->set_frame_sequencer(frame_sequencer);
    }

    channels[0]->tick();
    channels[1]->tick();
    channels[2]->tick();
    channels[3]->tick();

    if (--frequency_counter <= 0) {
        frequency_counter = 95;

        int left = 0, right = 0;
        byte output;

        for (int i = 0; i < 4; i++) {
            output = channels[i]->get_output() * volume[i];

            if (left_enables[i])
                left += output;
            if (right_enables[i])
                right += output;
        }

        if (audio_driver != nullptr)
            audio_driver->add_sample(convert_sample(left), convert_sample(right));
    }
}

byte Apu::read(word address) {
    if (address >= 0xFF10 && address <= 0xFF14)
        return channels[0]->read(address);

    else if (address >= 0xFF15 && address <= 0xFF19)
        return channels[1]->read(address);

    else if (address >= 0xFF1A && address <= 0xFF1E)
        return channels[2]->read(address);

    else if (address >= 0xFF1F && address <= 0xFF23)
        return channels[3]->read(address);

    else if (address >= 0xFF27 && address <= 0xFF2F)
        return 0xFF;

    else if (address >= 0xFF30 && address <= 0xFF3F)
        return channels[2]->read(address);

    byte result = 0;

    switch (address) {
        case 0xFF24:
            return (vin_left_enable ? BIT7 : 0) | (left_volume << 4) | (vin_right_enable ? BIT3 : 0) | right_volume;

        case 0xFF25:
            for (int i = 0; i < 4; i++) {
                result |= right_enables[i] ? (1  << i) : 0;
                result |= left_enables [i] ? (16 << i) : 0;
            }

            return result;

        case 0xFF26:
            result = enabled ? BIT7 : 0;

            for (int i = 0; i < 4; i++)
                result |= channels[i]->is_enabled() ? (1 << i) : 0;

            return result | 0x70;

        default:
            break;
    }

    LOG_WARNING("Apu::read can't access address 0x" + hex(address, 4));

    return 0x0;
}

void Apu::write(word address, byte value) {
    if (address == 0xFF26) {
        bool enable = (value & BIT7) != 0;

        // Clear registers when powered off
        if (enabled && !enable)
            clear_regs();
        // Reset Frame-Sequencer when powered on
        else if (!enabled & enable)
            frame_sequencer = 0;

        enabled = enable;
        return;
    }

    else if (address >= 0xFF30 && address <= 0xFF3F) {
        channels[2]->write(address, value);
        return;
    }

    if (!enabled) {
        if (gb->gbc_mode)
            return;

        // Power off does not affect length-counter writes
        switch (address) {
            case 0xFF11:
                channels[0]->write(address, value & 0x3F);
                return;

            case 0xFF16:
                channels[1]->write(address, value & 0x3F);
                return;

            case 0xFF1B:
                channels[2]->write(address, value);
                return;

            case 0xFF20:
                channels[3]->write(address, value & 0x3F);
                return;
        }

        return;
    }

    if (address >= 0xFF10 && address <= 0xFF14) {
        channels[0]->write(address, value);
        return;
    }

    else if (address >= 0xFF15 && address <= 0xFF19) {
        channels[1]->write(address, value);
        return;
    }

    else if (address >= 0xFF1A && address <= 0xFF1E) {
        channels[2]->write(address, value);
        return;
    }

    else if (address >= 0xFF1F && address <= 0xFF23) {
        channels[3]->write(address, value);
        return;
    }

    if (address >= 0xFF27 && address <= 0xFF2F)
        return;

    switch (address) {
        case 0xFF24:
            right_volume = value & 0b111;
            vin_right_enable = (value & BIT3) != 0;

            left_volume = (value >> 4) & 0b111;
            vin_left_enable  = (value & BIT7) != 0;
            return;

        case 0xFF25:
            for (int i = 0; i < 4; i++) {
                right_enables[i] = ((value >> i)     & 1) != 0;
                left_enables [i] = ((value >> (i+4)) & 1) != 0;
            }
            return;

        default:
            break;
    }

    LOG_WARNING("Apu::write can't access address 0x" + hex(address, 4));
}

void Apu::save_state(State &state) {
    channels[0]->save_state(state);
    channels[1]->save_state(state);
    channels[2]->save_state(state);
    channels[3]->save_state(state);

    state.write_data(left_enables,  4);
    state.write_data(right_enables, 4);

    state.write8(vin_left_enable);
    state.write8(vin_right_enable);

    state.write8(left_volume);
    state.write8(right_volume);

    state.write8(enabled);

    state.write32(frequency_counter);
    state.write32(frame_sequencer_counter);
    state.write32(frame_sequencer);
}

void Apu::load_state(State &state) {
    channels[0]->load_state(state);
    channels[1]->load_state(state);
    channels[2]->load_state(state);
    channels[3]->load_state(state);

    state.read_data(left_enables,  4);
    state.read_data(right_enables, 4);

    vin_left_enable  = state.read8();
    vin_right_enable = state.read8();

    left_volume   = state.read8();
    right_volume  = state.read8();

    enabled = state.read8();

    frequency_counter       = state.read32();
    frame_sequencer_counter = state.read32();
    frame_sequencer         = state.read32();
}

void Apu::load_settings(Settings &settings) {
    volume[0] = settings.audio_channel1_volume / 100.0f;
    volume[1] = settings.audio_channel2_volume / 100.0f;
    volume[2] = settings.audio_channel3_volume / 100.0f;
    volume[3] = settings.audio_channel4_volume / 100.0f;
}

void Apu::clear_regs() {
    vin_left_enable = vin_right_enable = 0;
    left_volume = right_volume = 0;

    enabled = 0;

    for (Channel *channel : channels)
        channel->power_off();

    for (int i = 0; i < 4; i++) {
        left_enables [i] = 0;
        right_enables[i] = 0;
    }
}

s16 Apu::convert_sample(s16 sample) {
    return (sample-32) << 10;
}
