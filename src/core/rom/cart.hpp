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

#include <string>

class State;
class BinaryFile;

class Cart {
public:
    Cart();
    virtual ~Cart();

    void init();

    int load_ecart (const std::string &file_path);
    void save_ecart(const std::string &file_path);

    void load_data(BinaryFile &file);

    virtual unsigned int rom_size  () const = 0;
    virtual unsigned int ecart_size() const = 0;

    enum UsageType {
        UsageType_Data       = 1,
        UsageType_InstrStart = 2,
        UsageType_Instr      = 3
    };

    virtual byte read_byte(word address, UsageType usage) = 0;
    virtual void write_byte(word address, byte value) = 0;

    void set_rom_type(byte rom_type);

    void save_state(State &state);
    void load_state(State &state);

    const byte *get_usage() const;

    virtual int get_usage(word address) = 0;

    int load_usage(const std::string &file_name);
    int dump_usage(const std::string &file_name) const;

protected:
    byte *data;
    byte *ecart;

    byte *rom_usage;

    byte rom_type;
};
