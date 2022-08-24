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

#include "core/types.hpp"
#include "common/wave_file.hpp"

#include <string>
#include <vector>

class Settings;

class AudioDriver
{
public:
    AudioDriver();
    virtual ~AudioDriver() { }

    virtual int start(unsigned int sample_rate, unsigned int buffer_size) = 0;
    virtual void stop() = 0;

    virtual void pause(bool value) = 0;

    void add_sample(s16 left, s16 right);

    virtual void set_sync_to_audio(bool sync_to_audio) = 0;

    virtual void reset();

    int start_dumping_audio(const std::string &file_path);
    void stop_dumping_audio();

    bool is_dumping_audio() const;

    int get_sample_rate() const;
    int get_buffer_size() const;

    bool get_sync_to_audio() const;

    enum Mode {
        Mode_Normal,
        Mode_SlowMotion,
        Mode_Turbo,
    };

    void set_mode(Mode mode);

    // Volume is a number ranging from 0 to 100
    int get_volume() const;

    void add_to_volume(int amount);

    void load_settings(Settings &settings);

protected:
    virtual void internal_add_sample(s16 left, s16 right) = 0;

    unsigned int sample_rate;
    unsigned int buffer_size;

    bool sync_to_audio;

    std::vector <s16> audio_buffer;

private:
    void set_volume(float volume);

    Mode mode;
    bool skip;
    float volume;

    WaveFile wave_file;
    bool dump_audio;
};
