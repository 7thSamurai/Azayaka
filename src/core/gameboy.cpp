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

#include "gameboy.hpp"
#include "cpu/cpu.hpp"
#include "cpu/timer.hpp"
#include "memory/mmu.hpp"
#include "memory/dma.hpp"
#include "memory/hdma.hpp"
#include "memory/boot_rom.hpp"
#include "rom/rom.hpp"
#include "gpu/gpu.hpp"
#include "audio/apu.hpp"
#include "input/joypad.hpp"
#include "serial/serial.hpp"
#include "state.hpp"
#include "input/input.hpp"
#include "display/display.hpp"
#include "common/utils.hpp"
#include "settings.hpp"

GameBoy::GameBoy() {
    startup();
}

GameBoy::~GameBoy() {
    shutdown();
}

// Function to load a ROM
int GameBoy::load_rom(const std::string &path, std::string &error, bool dump_usage) {
    int code = rom->load_rom(path, error);
    if (code < 0)
        return code;

    gbc_mode = rom->is_gbc();
    rom_path = path;

    rom->set_dump_usage(dump_usage);

    return 0;
}

int GameBoy::load_rom_force_mode(const std::string &path, std::string &error, bool gbc_mode, bool dump_usage) {
    int code = rom->load_rom(path, error);
    if (code < 0)
        return code;

    this->gbc_mode = gbc_mode;
    rom_path = path;

    rom->set_dump_usage(dump_usage);

    return 0;
}

void GameBoy::init() {
    run_bios();
}

void GameBoy::reset() {
    shutdown();
    startup();

    std::string error;
    load_rom_force_mode(rom_path, error, gbc_mode);
}

// Function to run the GameBoy for 1 emulator frame
void GameBoy::run_frame() {
    gpu->clear_refresh();

    while(!gpu->needs_refresh())
        cpu->step();
}

void GameBoy::run_link_frame(GameBoy &gb2) {
    gpu->clear_refresh();
    gb2.gpu->clear_refresh();

    while (!gpu->needs_refresh()) {
        cpu->step();
        gb2.cpu->step();
    }
}

void GameBoy::run_link_frame2(GameBoy &gb2) {
    while (!gb2.gpu->needs_refresh()) {
        cpu->step();
        gb2.cpu->step();
    }
}

void GameBoy::run_until_blargg_done() {
    while (!cpu->is_blargg_done())
        cpu->step();

    run_frame(); // Make sure that the screen is fully rendered
}

void GameBoy::run_until_mooneye_done() {
    while (!cpu->is_mooneye_done())
        cpu->step();

    run_frame(); // Make sure that the screen is fully rendered
}

bool GameBoy::is_frame_done() {
    bool done = gpu->needs_refresh();
    gpu->clear_refresh();

    return done;
}

void GameBoy::bind_audio_driver(AudioDriver *audio_driver) {
    apu->bind_audio_driver(audio_driver);
}

void GameBoy::connect_gameboy_link(GameBoy &gb) {
    serial->set_serial_device(gb.serial);
    gb.serial->set_serial_device(serial);
}

const Color *GameBoy::get_screen_buffer() const {
    return gpu->get_screen_buffer();
}

void GameBoy::run_bios() {
    if (gbc_mode) {
        if (cgb_bios_path.empty())
            cpu->init_gbc_mode();

        else {
            boot_rom->load(cgb_bios_path);
            mmu->register_component(boot_rom, 0x200, 0x8FF);
            return;
        }
    }

    else {
        if (dmg_bios_path.empty())
            cpu->init_gb_mode();

        else {
            boot_rom->load(dmg_bios_path);
            return;
        }
    }

    // Setting memory values
    mmu->write_byte(0xFF10, 0x80); // NR10
    mmu->write_byte(0xFF11, 0xBF); // NR11
    mmu->write_byte(0xFF12, 0xF3); // NR12
    mmu->write_byte(0xFF14, 0xBF); // NR14
    mmu->write_byte(0xFF16, 0x3F); // NR21
    mmu->write_byte(0xFF17, 0x00); // NR22
    mmu->write_byte(0xFF19, 0xBF); // NR24
    mmu->write_byte(0xFF1A, 0x7F); // NR30
    mmu->write_byte(0xFF1B, 0xFF); // NR31
    mmu->write_byte(0xFF1C, 0x9F); // NR32
    mmu->write_byte(0xFF1E, 0xBF); // NR33
    mmu->write_byte(0xFF20, 0x00); // NR41
    mmu->write_byte(0xFF22, 0x00); // NR42
    mmu->write_byte(0xFF23, 0xBF); // NR43
    mmu->write_byte(0xFF24, 0x77); // NR50
    mmu->write_byte(0xFF25, 0xF3); // NR51
    mmu->write_byte(0xFF26, 0xF1); // NR52
    mmu->write_byte(0xFF40, 0x91); // LCDC
    mmu->write_byte(0xFF50, 0x01); // BOOT ROM on/off
}

std::string GameBoy::get_rom_name() {
    return rom->get_rom_name();
}

std::string GameBoy::get_rom_type() {
    return rom->get_rom_type();
}

std::string GameBoy::get_rom_size() {
    return rom->get_rom_size();
}

std::string GameBoy::get_ram_size() {
    return rom->get_ram_size();
}

int GameBoy::get_version() {
    return rom->get_version();
}

std::string GameBoy::get_des() {
    return rom->get_des();
}

std::string GameBoy::get_checksum() {
    return rom->get_checksum();
}

bool GameBoy::get_logo_match() {
    return rom->get_logo_match();
}

std::string GameBoy::get_licensee_code() {
    return rom->get_licensee_code();
}

bool GameBoy::get_gbc_mode() {
    return gbc_mode;
}

void GameBoy::save_state(State &state) {
    cpu   ->save_state(state);
    mmu   ->save_state(state);
    dma   ->save_state(state);
    hdma  ->save_state(state);
    rom   ->save_state(state);
    gpu   ->save_state(state);
    apu   ->save_state(state);
    timer ->save_state(state);
    joypad->save_state(state);
    serial->save_state(state);
}

void GameBoy::load_state(State &state) {
    cpu   ->load_state(state);
    mmu   ->load_state(state);
    dma   ->load_state(state);
    hdma  ->load_state(state);
    rom   ->load_state(state);
    gpu   ->load_state(state);
    apu   ->load_state(state);
    timer ->load_state(state);
    joypad->load_state(state);
    serial->load_state(state);
}

int GameBoy::save_state(const std::string &path) {
    State_File state;

    if (state.open_write(path) < 0)
        return -1;

    save_state(state);
    state.close();

    return 0;
}

int GameBoy::load_state(const std::string &path) {
    State_File state;

    if (state.open_read(path) < 0)
        return -1;

    load_state(state);
    state.close();

    return 0;
}

void GameBoy::bind_input(Input &input) {
    input.bind_joypad(joypad);
}

void GameBoy::bind_serial_device(SerialDevice *serial_device) {
    serial->set_serial_device(serial_device);
}

void GameBoy::load_settings(Settings &settings) {
    dmg_bios_path = settings.bios_dmg_path;
    cgb_bios_path = settings.bios_cgb_path;

    apu->load_settings(settings);
}

void GameBoy::startup() {
    cpu      = new Cpu    (this);
    mmu      = new Mmu    (this);
    dma      = new Dma    (this);
    hdma     = new Hdma   (this);
    boot_rom = new BootRom(this);
    rom      = new Rom    (this);
    gpu      = new Gpu    (this);
    apu      = new Apu    (this);
    timer    = new Timer  (this);
    joypad   = new Joypad (this);
    serial   = new Serial (this);

    mmu->register_component(boot_rom, 0x0000, 0x00FF);
    mmu->register_component(rom,      0x0100, 0x7FFF);
    mmu->register_component(gpu,      0x8000, 0x9FFF);
    mmu->register_component(rom,      0xA000, 0xBFFF);
    mmu->register_component(gpu,      0xFE00, 0xFE9F);
    mmu->register_component(joypad,   0xFF00, 0xFF00);
    mmu->register_component(serial,   0xFF01, 0xFF02);
    mmu->register_component(timer,    0xFF04, 0xFF07);
    mmu->register_component(apu,      0xFF10, 0xFF3F);
    mmu->register_component(gpu,      0xFF40, 0xFF45);
    mmu->register_component(dma,      0xFF46, 0xFF46);
    mmu->register_component(gpu,      0xFF47, 0xFF4B);
    mmu->register_component(gpu,      0xFF4F, 0xFF4F);
    mmu->register_component(boot_rom, 0xFF50, 0xFF50);
    mmu->register_component(hdma,     0xFF51, 0xFF55);
    mmu->register_component(gpu,      0xFF68, 0xFF6B);
}

void GameBoy::shutdown() {
    delete cpu;
    delete mmu;
    delete dma;
    delete hdma;
    delete boot_rom;
    delete rom;
    delete gpu;
    delete apu;
    delete timer;
    delete joypad;
    delete serial;
}
