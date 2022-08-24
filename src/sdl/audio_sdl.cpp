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

#include "sdl/audio_sdl.hpp"
#include "common/logger.hpp"

#include <SDL.h>
#include <SDL_mutex.h>

static void audio_callback(void *userdata, Uint8 *stream, int len) {
    ((AudioSDL*)userdata)->callback(stream, len);
}

AudioSDL::AudioSDL() {
    started = 0;
}

int AudioSDL::start(unsigned int sample_rate, unsigned int buffer_size) {
    this->sample_rate = sample_rate;
    this->buffer_size = buffer_size;

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        LOG_ERROR("Unable to initialize audio subsystem");
        return -1;
    }
    else {
        LOG_DEBUG("Initialized audio subsystem");
    }

    mutex = SDL_CreateMutex();

    SDL_AudioSpec desired;
    desired.freq     = sample_rate;
    desired.format   = AUDIO_S16SYS;
    desired.channels = 2;
    desired.samples  = buffer_size;
    desired.callback = audio_callback;
    desired.userdata = this;

    SDL_AudioSpec obtained;

    device_id = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);

    if (device_id == 0) {
        LOG_ERROR("Unable to open audio-device: " + std::string(SDL_GetError()));
        return -1;
    }
    else
        LOG_DEBUG("Opened audio-device");

    started = 1;

    return 0;
}

void AudioSDL::stop() {
    if (!started)
        return;

    started = 0;

    SDL_PauseAudioDevice(device_id, 1);
    SDL_CloseAudioDevice(device_id);

    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    SDL_DestroyMutex(mutex);

    LOG_DEBUG("Stopped audio-device");
}

void AudioSDL::pause(bool value) {
    SDL_PauseAudioDevice(device_id, value);

    if (value)
        LOG_DEBUG("Paused audio-device");
    else
        LOG_DEBUG("Unpaused audio-device");
}

void AudioSDL::reset() {
    AudioDriver::reset();

    SDL_LockAudioDevice(device_id);
    audio_buffer.clear();
    SDL_UnlockAudioDevice(device_id);
}

void AudioSDL::set_sync_to_audio(bool sync_to_audio) {
    SDL_LockAudioDevice(device_id);

    audio_buffer.clear();
    this->sync_to_audio = sync_to_audio;

    SDL_UnlockAudioDevice(device_id);

    if (sync_to_audio)
        LOG_DEBUG("Syncing emulation to audio");
    else
        LOG_DEBUG("Not syncing emulation to audio");
}

void AudioSDL::callback(uint8_t *stream, int len) {
    SDL_LockMutex(mutex);

    s16 *audio_stream = (s16*)stream;
    len >>= 1;

    int size = audio_buffer.size();

    if (size > len)
        size = len;
    else if (size < len)
        std::fill(audio_stream+size, audio_stream+len, 0);

    std::copy(audio_buffer.begin(), audio_buffer.begin()+size, audio_stream);
    audio_buffer.erase(audio_buffer.begin(), audio_buffer.begin()+size);

    SDL_UnlockMutex(mutex);
}

void AudioSDL::internal_add_sample(int16_t left, int16_t right) {
    if (sync_to_audio) {
        while ((audio_buffer.size() >> 1) > buffer_size)
            SDL_Delay(1);
    }

    audio_buffer.push_back(left);
    audio_buffer.push_back(right);
}
