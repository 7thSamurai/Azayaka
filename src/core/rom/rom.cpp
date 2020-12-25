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

#include "rom.hpp"
#include "mbc1.hpp"
#include "mbc2.hpp"
#include "mbc3.hpp"
#include "mbc5.hpp"
#include "defs.hpp"
#include "gameboy.hpp"
#include "state.hpp"
#include "globals.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"
#include "common/binary_file.hpp"
#include "common/file_utils.hpp"

#include <ctime>

Rom::Rom(GameBoy *gb) : Component(gb) {
    cart = nullptr;

    dump_usage = false;
}

Rom::~Rom() {
    if (cart != nullptr) {
        cart->save_ecart(File::remove_extension(path));

        if (dump_usage)
            cart->dump_usage(File::remove_extension(path));

        delete cart;
    }
}

int Rom::load_rom(const std::string &rom_path, std::string &error) {
    path = rom_path;

    BinaryFile file(rom_path, BinaryFile::Mode_Read);

    if (!file.is_open()) {
        error = "Unable to open file";
        return -1;
    }

    unsigned int size = file.size();

    if (size < 0x14F)  {
        error = "File too small";
        return -2; // File too small
    }

    file.read(header, 0x14F);
    file.seek(0, BinaryFile::Pos_Beg);

    byte rom_type = header[0x147];
    rom_size_num = (32 * 1024) << header[0x148];

    switch (header[0x149]) {
        case 0: ram_size_num = (0   * 1024); break;
        case 1: ram_size_num = (2   * 1024); break;
        case 2: ram_size_num = (8   * 1024); break;
        case 3: ram_size_num = (32  * 1024); break;
        case 4: ram_size_num = (128 * 1024); break;
        case 5: ram_size_num = (64  * 1024); break;

        default:
            logger.log("Rom::load_rom unknown ram-size 0x" + hex(header[0x149], 2), Logger::Warning);
    }

    if (cart != nullptr)
        delete cart;

    switch (rom_type) {
        case 0x00:
            cart = new Plain;
            break;

        case 0x1:
        case 0x2:
        case 0x3:
            cart = new Mbc1(rom_size_num, ram_size_num);
            break;

        case 0x5:
        case 0x6:
            cart = new Mbc2(rom_size_num, ram_size_num);
            break;

        case 0x0F:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
            cart = new Mbc3(rom_size_num, ram_size_num);
            break;

        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
            cart = new Mbc5(rom_size_num, ram_size_num);
            break;

        default:
            file.close();
            error = "Unknown Rom-Type: 0x" + hex(rom_type, 2);
            return -1;
    }

    if (size > cart->rom_size()) {
        error = "File too big";
        return -1;
    }

    else if (size < 0x8000) {
        error = "File too small";
        return -1;
    }

    // Load the cart data
    cart->init();
    cart->load_data(file);
    cart->set_rom_type(rom_type);

    logo_match = true;

    for (int num = 0x104; num <= 0x133; num++) {
        if (header[num] != logo[num-0x104]) {
            logo_match = false;
            break;
        }
    }

    if (!logo_match)
        logger.log("Nintendo logo doesn't match. The ROM will not boot on a real GameBoy.", Logger::Warning);

    checksum = 0x19;

    for (int i = 0x134; i <= 0x14D; i++)
        checksum += header[i];

    if (checksum)
        logger.log("Checksum is incorrect. You may have a bad ROM dump.", Logger::Warning);

    if (ram_size_num > 0 || rom_type == 0x06)
        cart->load_ecart(File::remove_extension(path));

    cart->load_usage(File::remove_extension(path));

    return 0;
}

byte Rom::read(word address) {
    return cart->read_byte(address, Cart::UsageType_Data);
}

void Rom::write(word address, byte value) {
    cart->write_byte(address, value);
}

byte Rom::read_instruction(word address) {
    return cart->read_byte(address, Cart::UsageType_InstrStart);
}

byte Rom::read_operand(word address) {
    return cart->read_byte(address, Cart::UsageType_Instr);
}

const byte *Rom::get_rom_usage() const {
    return cart->get_usage();
}

int Rom::get_rom_usage(word address) const {
    return cart->get_usage(address);
}

std::string Rom::get_rom_type() const {
    switch (header[0x147]) {
        case 0x00: return "ROM ONLY";
        case 0x01: return "MBC1";
        case 0x02: return "MBC1+RAM";
        case 0x03: return "MBC1+RAM+BATT";
        case 0x05: return "MBC2";
        case 0x06: return "MBC2+BATT";
        case 0x08: return "ROM+RAM";
        case 0x09: return "RAOM+RAM+BATT";
        case 0x0B: return "MMM01";
        case 0x0C: return "MMM01+RAM";
        case 0x0D: return "MMM01+RAM+BATT";
        case 0x0F: return "MBC3+TIMER+BATT";
        case 0x10: return "MBC3+TIMER+RAM+BATT";
        case 0x11: return "MBC3";
        case 0x12: return "MBC3+RAM";
        case 0x13: return "MBC3+RAM+BATT";
        case 0x15: return "MBC4";
        case 0x16: return "MBC4+RAM";
        case 0x17: return "MBC4+RAM+BATT";
        case 0x19: return "MBC5";
        case 0x1A: return "MBC5+RAM";
        case 0x1B: return "MBC5+RAM+BATT";
        case 0x1C: return "MBC5+RUMBLE";
        case 0x1D: return "MBC5+RUMBLE+RAM";
        case 0x1E: return "MBC5+RUMBLE+RAM+BATT";
        case 0xFC: return "POCKET-CAMERA";
        case 0xFD: return "BANDI TAMA5";
        case 0xFE: return "HuC3";
        case 0xFF: return "HuC1+RAM+BATT";

        default:
            break;
    }

    return "UNKNOWN";
}

std::string Rom::get_rom_size() const {
    switch (header[0x148]) {
        case 0: return "32 KB";
        case 1: return "64 KB";
        case 2: return "128 KB";
        case 3: return "256 KB";
        case 4: return "512 KB";
        case 5: return "1 MB";
        case 6: return "2 MB";
        case 7: return "4 MB";
        case 9: return "8 MB";

        default:
            break;
    }

    return "Invalid-Size";
}

std::string Rom::get_ram_size() const {
    switch (header[0x149]) {
        case 0: return "NONE";
        case 1: return "2 KB";
        case 2: return "8 KB";
        case 3: return "32 KB";
        case 4: return "12 8KB";
        case 5: return "64 KB";

        default:
            break;
    }

    return "Invalid-Size";
}

std::string Rom::get_checksum() const {
    return (checksum == 0) ? "Passed" : "Failed";
}

bool Rom::is_gbc() const {
    return header[0x143] == 0xC0 || header[0x143] == 0x80;
}

bool Rom::is_sgb() const {
    return header[0x146] == 0x03 && header[0x14B] == 0x33;
}

int Rom::get_version() const {
    return header[0x14C];
}

std::string Rom::get_des() const {
    switch (header[0x14A]) {
        case 0: return "Japan";
        case 1: return "Non-Japanese";

        default:
            break;
    }

    return "Unknown";
}

bool Rom::get_logo_match() const {
    return logo_match;
}

std::string Rom::get_rom_name() {
    std::string name = "";
    int size = is_gbc() ? 11 : 16;

    for (int i = 0; i < size; i++) {
        if (header[0x134+i] == 0)
            break;

        name += header[0x134+i];
    }

    return name;
}

std::string Rom::get_licensee_code() {
    if (header[0x14B] == 0x33)
        return std::string(1, header[0x144]) + std::string(1, header[0x145]);

    return std::string(1, header[0x14B]);
}

void Rom::save_state(State &state) {
    cart->save_state(state);
}

void Rom::load_state(State &state) {
    cart->load_state(state);
}


bool Rom::is_mbc() const {
    return dynamic_cast<Mbc*>(cart) != nullptr;
}

bool Rom::has_bat() const {
    return header[0x147] == 0x03 || header[0x147] == 0x06 ||
           header[0x147] == 0x0F || header[0x147] == 0x10 ||
           header[0x147] == 0x13 || header[0x147] == 0x1B ||
           header[0x147] == 0x1E;
}

bool Rom::has_rtc() const {
    return header[0x147] == 0x0F || header[0x147] == 0x10;
}

bool Rom::has_rumble() const {
    return header[0x147] == 0x1C || header[0x147] == 0x1D || header[0x147] == 0x1E;
}

bool Rom::get_mbc_ram_on() const {
    return (dynamic_cast<Mbc*>(cart))->ram_enabled();
}

int Rom::get_mbc_rom_bank() const {
    return (dynamic_cast<Mbc*>(cart))->get_rom_bank();
}

int Rom::get_mbc_ram_bank() const {
    return (dynamic_cast<Mbc*>(cart))->get_ram_bank();
}

bool Rom::is_mbc1() const {
    return dynamic_cast<Mbc1*>(cart) != nullptr;
}

bool Rom::get_mbc1_mode() const {
    return (dynamic_cast<Mbc1*>(cart))->get_mode();
}

void Rom::set_dump_usage(bool dump_usage) {
    this->dump_usage = dump_usage;
}

Plain::Plain() : Cart() {
}

unsigned int Plain::rom_size() const {
    return 0x8000;
}

unsigned int Plain::ecart_size() const {
    return (8 * 1024);
}

byte Plain::read_byte(word address, UsageType usage) {
    if (address <= 0x7FFF) {
        rom_usage[address] = usage;
        return data[address];
    }

    else if (address >= 0xA000 && address <= 0xBFFF)
        return ecart[address - 0xA000];

    logger.log("Plain::read_byte can't access address 0x" + hex(address, 4), Logger::Warning);

    return 0;
}

void Plain::write_byte(word address, byte value) {
    if (address >= 0xA000 && address <= 0xBFFF)
        ecart[address - 0xA000] = value;

    else
        logger.log("Plain::write_byte can't access address 0x" + hex(address, 4), Logger::Warning);
}

int Plain::get_usage(word address) {
    return rom_usage[address];
}
