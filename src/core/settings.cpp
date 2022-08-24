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

#include "core/settings.hpp"
#include "common/ini_file.hpp"

#include "core/gameboy.hpp"
#include "core/audio/audio_driver.hpp"
#include "core/display/display.hpp"
#include "core/input/input.hpp"
#include "core/rewinder.hpp"

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
    input1_a      = "Z";
    input1_b      = "X";
    input1_start  = "Return";
    input1_select = "Space";
    input1_up     = "Up";
    input1_down   = "Down";
    input1_left   = "Left";
    input1_right  = "Right";

    input2_a      = "A";
    input2_b      = "S";
    input2_start  = "Keypad Enter";
    input2_select = "Keypad 0";
    input2_up     = "Keypad Up";
    input2_down   = "Keypad Down";
    input2_left   = "Keypad Left";
    input2_right  = "Keypad Right";

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

void Settings::configure(GameBoy &gameboy, Display &display, AudioDriver &audio_driver, Input &input1, Input &input2, Rewinder &rewinder) {
    gameboy.load_settings(*this);
    display.load_settings(*this);
    audio_driver.load_settings(*this);
    input1.load_settings(*this, 0);
    input2.load_settings(*this, 1);

    rewinder.set_length(emu_rewind_length);
}

void Settings::load_audio(IniFile &ini) {
    IniFile::Section *audio = ini.get_section("Audio");

    if (audio) {
        audio_sample_rate     = audio->get_int("SampleRate");
        audio_buffer_size     = audio->get_int("BufferSize");
        audio_master_volume   = audio->get_int("MasterVolume");
        audio_channel1_volume = audio->get_int("Channel1Volume");
        audio_channel2_volume = audio->get_int("Channel2Volume");
        audio_channel3_volume = audio->get_int("Channel3Volume");
        audio_channel4_volume = audio->get_int("Channel4Volume");
    }
}

void Settings::load_bios(IniFile &ini) {
    IniFile::Section *bios = ini.get_section("Bios");

    if (bios) {
        bios_dmg_path = bios->get_str("DmgPath");
        bios_cgb_path = bios->get_str("CgbPath");
    }
}

void Settings::load_input(IniFile &ini) {
    IniFile::Section *input1 = ini.get_section("Input1");
    IniFile::Section *input2 = ini.get_section("Input2");

    if (input1) {
        input1_a      = input1->get_str("A");
        input1_b      = input1->get_str("B");
        input1_start  = input1->get_str("Start");
        input1_select = input1->get_str("Select");
        input1_up     = input1->get_str("Up");
        input1_down   = input1->get_str("Down");
        input1_left   = input1->get_str("Left");
        input1_right  = input1->get_str("Right");
    }

    if (input2) {
        input2_a      = input2->get_str("A");
        input2_b      = input2->get_str("B");
        input2_start  = input2->get_str("Start");
        input2_select = input2->get_str("Select");
        input2_up     = input2->get_str("Up");
        input2_down   = input2->get_str("Down");
        input2_left   = input2->get_str("Left");
        input2_right  = input2->get_str("Right");
    }
}

void Settings::load_video(IniFile &ini) {
    IniFile::Section *video = ini.get_section("Video");

    if (video)
        video_lock_aspect_ratio = video->get_bool("LockAspectRatio");
}

void Settings::load_emulation(IniFile &ini) {
    IniFile::Section *emulation = ini.get_section("Emulation");

    if (emulation) {
        emu_rewind_length = emulation->get_int("RewindLength");
        emu_sync_to_audio = emulation->get_bool("SyncToAudio");
    }
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
    IniFile::Section *input1 = ini.get_or_create_section("Input1");
    IniFile::Section *input2 = ini.get_or_create_section("Input2");

    input1->set_str("A",      input1_a);
    input1->set_str("B",      input1_b);
    input1->set_str("Start",  input1_start);
    input1->set_str("Select", input1_select);
    input1->set_str("Up",     input1_up);
    input1->set_str("Down",   input1_down);
    input1->set_str("Left",   input1_left);
    input1->set_str("Right",  input1_right);

    input2->set_str("A",      input2_a);
    input2->set_str("B",      input2_b);
    input2->set_str("Start",  input2_start);
    input2->set_str("Select", input2_select);
    input2->set_str("Up",     input2_up);
    input2->set_str("Down",   input2_down);
    input2->set_str("Left",   input2_left);
    input2->set_str("Right",  input2_right);
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
