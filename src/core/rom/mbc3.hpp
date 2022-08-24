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

#include "core/rom/mbc.hpp"
#include "core/rom/rtc.hpp"

class Mbc3 : public Mbc
{
public:
    Mbc3(int rom_size_num, int ram_size_num);

    unsigned int rom_size  () const override;
    unsigned int ecart_size() const override;

    byte read_byte(word address, UsageType usage) override;
    void write_byte(word address, byte value) override;

    int load_timer(const std::string &file_path);
    int save_timer(const std::string &file_path);

protected:
    //void mbc_save_state(State &state) override;
    //void mbc_load_state(State &state) override;

private:
    Rtc rtc;
};
