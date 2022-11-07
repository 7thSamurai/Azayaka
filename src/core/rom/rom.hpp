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
#include "core/component.hpp"
#include "core/rom/cart.hpp"

#include <string>
#include <fstream>

class Cart;
class State;

class Rom : public Component
{
public:
    Rom (GameBoy *gb);
    ~Rom();

    int load_rom(const std::string &rom_path, std::string &error);

    byte read(word address) override;
    void write(word address, byte value) override;

    byte read_instruction(word address);
    byte read_operand    (word address);

    const byte *get_rom_usage() const;
    int get_rom_usage(word address) const;

    std::string get_rom_type() const;
    std::string get_rom_size() const;
    std::string get_ram_size() const;
    std::string get_checksum() const;

    bool is_gbc() const;
    bool is_sgb() const;

    int get_version    () const;
    std::string get_des() const;
    bool get_logo_match() const;

    std::string get_rom_name();
    std::string get_licensee_code();

    void save_state(State &state);
    void load_state(State &state);

    bool is_mbc    () const;
    bool has_bat   () const;
    bool has_rtc   () const;
    bool has_rumble() const;

    bool get_mbc_ram_on () const;
    int get_mbc_rom_bank() const;
    int get_mbc_ram_bank() const;

    bool is_mbc1() const;
    bool get_mbc1_mode() const;

    void set_dump_usage(bool dump_usage);
    void set_save_enabled(bool enabled);

private:
    const byte logo[48] = {
        0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
        0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
        0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
    };

    std::string ram_size_string;

    unsigned int rom_size_num;
    unsigned int ram_size_num;

    byte header[0x14F];

    byte checksum;
    bool logo_match;

    std::string path;

    Cart *cart;
    bool dump_usage;
    bool save_enabled;
};

class Plain : public Cart {
public:
    Plain();

    unsigned int rom_size() const;
    unsigned int ecart_size() const;

    byte read_byte(word address, UsageType usage);
    void write_byte(word address, byte value);

    int get_usage(word address);
};
