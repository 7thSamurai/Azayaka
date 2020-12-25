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

#include "settings.hpp"
#include "common/ini_file.hpp"

#include "gameboy.hpp"
#include "audio/audio_driver.hpp"
#include "display/display.hpp"
#include "input/input.hpp"
#include "rewinder.hpp"

Settings::Settings() {
    // Audio
    audio_sample_rate     = 44100;
    audio_buffer_size     = 4096;
    audio_master_volume   = 100;
    audio_channel1_volume = 100;
    audio_channel2_volume = 100;
    audio_channel3_volume = 100;
    audio_channel4_volume = 100;

    // BIOS
    bios_dmg_path = "";
    bios_cgb_path = "";

    // Input
    input_a      = "Z";
    input_b      = "X";
    input_start  = "Return";
    input_select = "Space";
    input_up     = "Up";
    input_down   = "Down";
    input_left   = "Left";
    input_right  = "Right";

    // Video
    video_lock_aspect_ratio = true;

    // Emulation
    emu_rewind_length = 5 * 60;
    emu_sync_to_audio = true;
}

int Settings::load(const std::string &file_path) {
    IniFile ini;
    if (ini.load(file_path) < 0)
        return -1;

    load_audio(ini);
    load_bios(ini);
    load_input(ini);
    load_video(ini);
    load_emulation(ini);

    return 0;
}

int Settings::save(const std::string &file_path) {
    IniFile ini;

    save_audio(ini);
    save_bios(ini);
    save_input(ini);
    save_video(ini);
    save_emulation(ini);

    return ini.save(file_path);
}

void Settings::configure(GameBoy &gameboy, Display &display, AudioDriver &audio_driver, Input &input, Rewinder &rewinder) {
    gameboy.load_settings(*this);
    display.load_settings(*this);
    audio_driver.load_settings(*this);
    input.load_settings(*this);

    rewinder.set_length(emu_rewind_length);
}

void Settings::load_audio(IniFile &ini) {
    IniFile::Section *audio = ini.get_section("Audio");

    audio_sample_rate     = audio->get_int("SampleRate");
    audio_buffer_size     = audio->get_int("BufferSize");
    audio_master_volume   = audio->get_int("MasterVolume");
    audio_channel1_volume = audio->get_int("Channel1Volume");
    audio_channel2_volume = audio->get_int("Channel2Volume");
    audio_channel3_volume = audio->get_int("Channel3Volume");
    audio_channel4_volume = audio->get_int("Channel4Volume");
}

void Settings::load_bios(IniFile &ini) {
    IniFile::Section *bios = ini.get_section("Bios");

    bios_dmg_path = bios->get_str("DmgPath");
    bios_cgb_path = bios->get_str("CgbPath");
}

void Settings::load_input(IniFile &ini) {
    IniFile::Section *input = ini.get_section("Input");

    input_a      = input->get_str("A");
    input_b      = input->get_str("B");
    input_start  = input->get_str("Start");
    input_select = input->get_str("Select");
    input_up     = input->get_str("Up");
    input_down   = input->get_str("Down");
    input_left   = input->get_str("Left");
    input_right  = input->get_str("Right");
}

void Settings::load_video(IniFile &ini) {
    IniFile::Section *video = ini.get_section("Video");

    video_lock_aspect_ratio = video->get_bool("LockAspectRatio");
}

void Settings::load_emulation(IniFile &ini) {
    IniFile::Section *emulation = ini.get_section("Emulation");

    emu_rewind_length = emulation->get_int("RewindLength");
    emu_sync_to_audio = emulation->get_bool("SyncToAudio");
}


void Settings::save_audio(IniFile &ini) {
    IniFile::Section *audio = ini.get_or_create_section("Audio");

    audio->set_int("SampleRate",     audio_sample_rate);
    audio->set_int("BufferSize",     audio_buffer_size);
    audio->set_int("MasterVolume",   audio_master_volume);
    audio->set_int("Channel1Volume", audio_channel1_volume);
    audio->set_int("Channel2Volume", audio_channel2_volume);
    audio->set_int("Channel3Volume", audio_channel3_volume);
    audio->set_int("Channel4Volume", audio_channel4_volume);
}

void Settings::save_bios(IniFile &ini) {
    IniFile::Section *bios = ini.get_or_create_section("Bios");

    bios->set_str("DmgPath", bios_dmg_path);
    bios->set_str("CgbPath", bios_cgb_path);
}

void Settings::save_input(IniFile &ini) {
    IniFile::Section *input = ini.get_or_create_section("Input");

    input->set_str("A",      input_a);
    input->set_str("B",      input_b);
    input->set_str("Start",  input_start);
    input->set_str("Select", input_select);
    input->set_str("Up",     input_up);
    input->set_str("Down",   input_down);
    input->set_str("Left",   input_left);
    input->set_str("Right",  input_right);
}

void Settings::save_video(IniFile &ini) {
    IniFile::Section *video = ini.get_or_create_section("Video");

    video->set_bool("LockAspectRatio", video_lock_aspect_ratio);
}

void Settings::save_emulation(IniFile &ini) {
    IniFile::Section *emulation = ini.get_or_create_section("Emulation");

    emulation->set_int("RewindLength", emu_rewind_length);
    emulation->set_bool("SyncToAudio", emu_sync_to_audio);
}
