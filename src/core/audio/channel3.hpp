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

#ifndef _CHANNEL3_HPP_
#define _CHANNEL3_HPP_

#include "channel.hpp"

class Channel3 : public Channel
{
public:
    Channel3(GameBoy *gb);

    byte read(word address) override;
    void write(word address, byte value) override;

    void tick() override;
    void power_off() override;

    void save_state(State &state) override;
    void load_state(State &state) override;

private:
    byte wave_table[16];

    void trigger();

    int timer;
    int position;
    int ticks_since_read;
    int last_address;

    int frequency;
    byte volume_code;
};

#endif // _CHANNEL3_HPP_
