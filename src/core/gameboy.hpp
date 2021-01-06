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

#ifndef _GAME_BOY_HPP_
#define _GAME_BOY_HPP_

#include "types.hpp"
#include <string>

class Cpu;
class Mmu;
class Dma;
class Hdma;
class BootRom;
class Rom;
class Gpu;
class Apu;
class Timer;
class Joypad;
class Serial;
class Input;
class GbcReg;

class Display;
class AudioDriver;
class SerialDevice;
class State;
class Settings;

class GameBoy
{
public:
    GameBoy();
    ~GameBoy();

    int load_rom(const std::string &path, std::string &error, bool dump_usage = false);
    int load_rom_force_mode(const std::string &path, std::string &error, bool gbc_mode, bool dump_usage = false);

    void init();

    void reset();

    void run_frame();

    void run_until_blargg_done();
    void run_until_mooneye_done();

    bool is_frame_done();

    void bind_audio_driver(AudioDriver *audio_driver);

    const Color *get_screen_buffer() const;

    std::string get_rom_name();
    std::string get_rom_type();
    std::string get_rom_size();
    std::string get_ram_size();
    int get_version();
    std::string get_des();
    std::string get_checksum();
    bool get_logo_match();
    std::string get_licensee_code();
    bool get_gbc_mode();

    void save_state(State &state);
    void load_state(State &state);

    int save_state(const std::string &path);
    int load_state(const std::string &path);

    void bind_input(Input &input);
    void bind_serial_device(SerialDevice *serial_device);

    void load_settings(Settings &settings);

    Cpu *cpu;
    Mmu *mmu;
    Dma *dma;
    Hdma *hdma;
    BootRom *boot_rom;
    Rom *rom;
    Gpu *gpu;
    Apu *apu;
    Timer *timer;
    Joypad *joypad;
    Serial *serial;

    bool gbc_mode;

private:
    void startup();
    void shutdown();

    void run_bios();

    std::string rom_path;

    std::string dmg_bios_path;
    std::string cgb_bios_path;

    Display *display;
};

#endif // _GAME_BOY_HPP_
