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

#ifndef _APU_HPP_
#define _APU_HPP_

#include "component.hpp"
#include "types.hpp"
#include "channel.hpp"

class AudioDriver;
class State;
class Settings;

class Apu : public Component
{
public:
    Apu(GameBoy *gb);
    ~Apu();

    void bind_audio_driver(AudioDriver *audio_driver);

    void tick();

    byte read(word address) override;
    void write(word address, byte value) override;

    void save_state(State &state);
    void load_state(State &state);

    void load_settings(Settings &settings);

private:
    void clear_regs();
    s16 convert_sample(s16 sample);

    Channel *channels[4];
    float volume[4];

    bool left_enables[4], right_enables[4];

    bool vin_left_enable, vin_right_enable;
    byte left_volume, right_volume;

    bool enabled;

    int frequency_counter;

    int frame_sequencer_counter;
    int frame_sequencer;

    AudioDriver *audio_driver;
};

#endif // _APU_HPP_
