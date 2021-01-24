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

#ifndef _SETTINGS_HPP_
#define _SETTINGS_HPP_

#include <string>

class IniFile;

class GameBoy;
class Display;
class AudioDriver;
class Rewinder;
class Input;

class Settings
{
public:
    Settings();

    int load(const std::string &file_path);
    int save(const std::string &file_path);

    void configure(GameBoy &gameboy, Display &display, AudioDriver &audio_driver, Input &input1, Input &input2, Rewinder &rewinder);

    // Audio
    unsigned int audio_sample_rate;     // Sample-rate for audio playback
    unsigned int audio_buffer_size;     // Size of audio-buffer
    unsigned int audio_master_volume;   // Master volume
    unsigned int audio_channel1_volume; // Volume of Channel 1
    unsigned int audio_channel2_volume; // Volume of Channel 2
    unsigned int audio_channel3_volume; // Volume of Channel 3
    unsigned int audio_channel4_volume; // Volume of Channel 4

    // BIOS
    std::string bios_dmg_path; // GameBoy BIOS
    std::string bios_cgb_path; // GameBoy Color BIOS

    // Input
    std::string input1_a;
    std::string input1_b;
    std::string input1_start;
    std::string input1_select;
    std::string input1_up;
    std::string input1_down;
    std::string input1_left;
    std::string input1_right;

    std::string input2_a;
    std::string input2_b;
    std::string input2_start;
    std::string input2_select;
    std::string input2_up;
    std::string input2_down;
    std::string input2_left;
    std::string input2_right;

    // Video
    bool video_lock_aspect_ratio; // Lock the aspect-ratio of the display

    // Emulation
    unsigned int emu_rewind_length; // How many seconds you can rewind back
    bool emu_sync_to_audio;         // Sync emulation to audio


private:
    void load_audio    (IniFile &ini);
    void load_bios     (IniFile &ini);
    void load_input    (IniFile &ini);
    void load_video    (IniFile &ini);
    void load_emulation(IniFile &ini);

    void save_audio    (IniFile &ini);
    void save_bios     (IniFile &ini);
    void save_input    (IniFile &ini);
    void save_video    (IniFile &ini);
    void save_emulation(IniFile &ini);
};

#endif // _SETTINGS_HPP_
