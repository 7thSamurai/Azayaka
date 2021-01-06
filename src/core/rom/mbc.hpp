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

#ifndef _MBC_HPP_
#define _MBC_HPP_

#include "cart.hpp"

class Mbc : public Cart {
public:
    Mbc(int rom_size_num, int ram_size_num);

    bool ram_enabled() const;
    int get_ram_bank() const;
    int get_rom_bank() const;

    virtual void save_state(State &state);
    virtual void load_state(State &state);

    int get_usage(word address) override;

protected:
    u32 rom_offset;
    u32 ram_offset;

    bool ram_on;
    word rom_bank; // Mbc5 needs 16-bit
    byte ram_bank;

    word rom_bank_mask;
    word ram_bank_mask;
};

#endif // _MBC_HPP_
