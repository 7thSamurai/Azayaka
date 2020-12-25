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

#ifndef _AUDIO_SDL_HPP_
#define _AUDIO_SDL_HPP_

#include "audio/audio_driver.hpp"

#include <cstdint>

struct SDL_mutex;

class AudioSDL : public AudioDriver
{
public:
    AudioSDL();

    int start(unsigned int sample_rate, unsigned int buffer_size) override;
    void stop() override;

    void pause(bool value) override;
    void reset() override;

    void set_sync_to_audio(bool sync_to_audio) override;

    void callback(uint8_t *stream, int len);

private:
    void internal_add_sample(int16_t left, int16_t right) override;

    bool started;
    int device_id;

    SDL_mutex *mutex;
};

#endif // _AUDIO_SDL_HPP_
