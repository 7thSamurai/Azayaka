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

#ifndef _MBC1_HPP_
#define _MBC1_HPP_

#include "mbc.hpp"

class Mbc1 : public Mbc
{
public:
    Mbc1(int rom_size_num, int ram_size_num);

    unsigned int rom_size  () const override;
    unsigned int ecart_size() const override;

    byte read_byte(word address, UsageType usage) override;
    void write_byte(word address, byte value) override;

    void check_multicart();

    bool get_mode() const;

    void save_state(State &state) override;
    void load_state(State &state) override;

private:
    void update_rom_ram();
    byte get_hi_shift() const;

	const byte logo[48] = {
		0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
		0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
		0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
	};

    bool mode;
    byte hi_bank;

    bool is_multicart;
};

#endif // _MBC1_HPP_
