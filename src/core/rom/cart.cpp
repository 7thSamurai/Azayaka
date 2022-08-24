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

#include "core/rom/cart.hpp"
#include "core/rom/mbc1.hpp"
#include "core/rom/mbc2.hpp"
#include "core/rom/mbc3.hpp"
#include "core/rom/mbc5.hpp"
#include "core/state.hpp"
#include "common/logger.hpp"
#include "common/binary_file.hpp"
#include "common/file_utils.hpp"

#include <iostream>

Cart::Cart() {
    data  = nullptr;
    ecart = nullptr;

    rom_usage = nullptr;
}

Cart::~Cart() {
    // Free the data
    if (data != nullptr)
        delete[] data;
    if (ecart != nullptr)
        delete[] ecart;

    if (rom_usage != nullptr)
        delete[] rom_usage;
}

void Cart::init() {
    // Allocate the data
    data  = new byte[rom_size()];
    ecart = new byte[ecart_size()];

    rom_usage = new byte[rom_size()];

    // Clear the data
    std::fill(data,  data+rom_size   (), 0);
    std::fill(ecart, ecart+ecart_size(), 0);

    std::fill(rom_usage, rom_usage+rom_size(), 0);
}

int Cart::load_ecart(const std::string &file_path) {
    if (rom_type == 0x03 ||
        rom_type == 0x06 ||
        rom_type == 0x0F || rom_type == 0x10 || rom_type == 0x13 ||
        rom_type == 0x1B || rom_type == 0x1E ||
        rom_type == 0xFC)
    {
        if (!File::exists(file_path + ".sav"))
            return 0;

        BinaryFile file(file_path + ".sav", BinaryFile::Mode_Read);
        if(!file.is_open()) {
            LOG_ERROR("Cart::load_ecart Unable to open ECART");
            return -1;
        }

        unsigned int size = file.size();

        Mbc3 *mbc3 = dynamic_cast<Mbc3*>(this);
        if (mbc3 != nullptr && (rom_type == 0x0F || rom_type == 0x10)) {
            if (mbc3->load_timer(file_path+".rtc") < 0)
                LOG_ERROR("Cart::load_ecart Unable to load RTC");
        }

        if (size == ecart_size())
            file.read(ecart, size);
        else
            LOG_ERROR("Cart::load_ecart Invalid ECART file size");
    }

    return 0;
}

void Cart::save_ecart(const std::string &file_path) {
    if (rom_type == 0x03 ||
        rom_type == 0x06 ||
        rom_type == 0x0F || rom_type == 0x10 || rom_type == 0x13 ||
        rom_type == 0x1B || rom_type == 0x1E ||
        rom_type == 0xFC)
    {
        BinaryFile file(file_path + ".sav", BinaryFile::Mode_Write);

        if (!file.is_open()) {
            LOG_ERROR("Cart::save_ecart Unable to save ECART");
            return;
        }

        file.write(ecart, ecart_size());

        Mbc3 *mbc3 = dynamic_cast<Mbc3*>(this);
        if (mbc3 != nullptr && (rom_type == 0x0F || rom_type == 0x10)) {
            if (mbc3->save_timer(file_path + ".rtc") < 0)
                LOG_ERROR("Cart::save_ecart Unable to save RTC");
        }
    }
}

void Cart::load_data(BinaryFile &file) {
    file.read(data, file.size());

    Mbc1 *mbc1 = dynamic_cast<Mbc1*>(this);
    if (mbc1 != nullptr)
        mbc1->check_multicart();
}

void Cart::set_rom_type(byte rom_type) {
    this->rom_type = rom_type;
}

void Cart::save_state(State &state) {
    state.write_data(ecart, ecart_size());

    Mbc *mbc = dynamic_cast<Mbc*>(this);
    if (mbc != nullptr)
        mbc->save_state(state);
}

void Cart::load_state(State &state) {
    state.read_data(ecart, ecart_size());

    Mbc *mbc = dynamic_cast<Mbc*>(this);
    if (mbc != nullptr)
        mbc->load_state(state);
}

const byte *Cart::get_usage() const {
    return rom_usage;
}

int Cart::load_usage(const std::string &file_name) {
    BinaryFile file(file_name + ".usage", BinaryFile::Mode_Read);

    if (!file.is_open())
        return -1;

    // Make sure that it is the correct size
    if (file.size() != rom_size())
        return -1;

    file.read(rom_usage, rom_size());

    return 0;
}

int Cart::dump_usage(const std::string &file_name) const {
    BinaryFile file(file_name + ".usage", BinaryFile::Mode_Write);

    if (!file.is_open())
        return -1;

    file.write(rom_usage, rom_size());

    return 0;
}
