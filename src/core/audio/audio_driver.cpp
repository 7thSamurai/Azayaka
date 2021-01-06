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

#include "audio_driver.hpp"
#include "common/logger.hpp"
#include "globals.hpp"
#include "settings.hpp"

#include <cmath>

AudioDriver::AudioDriver() {
    mode = Mode_Normal;
    skip = false;

    volume = 1.0f;

    dump_audio = false;

    sync_to_audio = 1;
}

void AudioDriver::add_sample(s16 left, s16 right) {
    left  *= volume;
    right *= volume;

    // Yes, I know that this is really ugly, and contains a lot of
    // code-duplication but I am kinda uncreative and I couldn't think
    // of any better methods to do this that are reasonably fast...

    if (dump_audio) {
        switch (mode) {
            case Mode_Normal:
                internal_add_sample(left, right);
                wave_file.write_sample(left, right);
                break;

            case Mode_SlowMotion:
                internal_add_sample(left, right);
                internal_add_sample(left, right);

                wave_file.write_sample(left, right);
                wave_file.write_sample(left, right);
                break;

            case Mode_Turbo:
                skip = !skip;

                if (skip) {
                    internal_add_sample(left, right);
                    wave_file.write_sample(left, right);
                }
                break;
        }
    }

    else {
        switch (mode) {
            case Mode_Normal:
                internal_add_sample(left, right);
                break;

            case Mode_SlowMotion:
                internal_add_sample(left, right);
                internal_add_sample(left, right);
                break;

            case Mode_Turbo:
                skip = !skip;

                if (skip)
                    internal_add_sample(left, right);
                break;
        }
    }
}

int AudioDriver::start_dumping_audio(const std::string &file_path) {
    if (dump_audio)
        wave_file.close();

    if (wave_file.open(file_path, sample_rate)) {
        logger.log("Unable to dump audio", Logger::Error);
        return -1;
    }
    else
        logger.log("Starting to dump audio...", Logger::Info, Logger::Verbose);

    dump_audio = true;

    return 0;
}

void AudioDriver::stop_dumping_audio() {
    if (dump_audio) {
        wave_file.close();
        dump_audio = false;

        logger.log("Stopped dumping audio", Logger::Info, Logger::Verbose);
    }
}

bool AudioDriver::is_dumping_audio() const {
    return dump_audio;
}

int AudioDriver::get_sample_rate() const {
    return sample_rate;
}

int AudioDriver::get_buffer_size() const {
    return buffer_size;
}

bool AudioDriver::get_sync_to_audio() const {
    return sync_to_audio;
}

void AudioDriver::reset() {
    mode = Mode_Normal;
}

void AudioDriver::set_mode(Mode mode) {
    this->mode = mode;

    switch (mode) {
        case Mode_Normal:
            logger.log("Set audio-driver to run at normal-speed", Logger::Info, Logger::Verbose);
            break;

        case Mode_SlowMotion:
            logger.log("Set audio-driver to run at half-speed", Logger::Info, Logger::Verbose);
            break;

        case Mode_Turbo:
            logger.log("Set audio-driver to run at double-speed", Logger::Info, Logger::Verbose);
            break;
    }
}

int AudioDriver::get_volume() const {
    return round(volume * 100.0f);
}

void AudioDriver::add_to_volume(int amount) {
    set_volume(volume + amount/100.0f);
}

void AudioDriver::load_settings(Settings &settings) {
    stop();
    start(settings.audio_sample_rate, settings.audio_buffer_size);

    volume = settings.audio_master_volume / 100.0f;
    sync_to_audio = settings.emu_sync_to_audio;
}

void AudioDriver::set_volume(float volume) {
    // Clamp
    if (volume < 0.0f)
        volume = 0.0f;

    if (volume > 1.0f)
        volume = 1.0f;

    this->volume = volume;
}
