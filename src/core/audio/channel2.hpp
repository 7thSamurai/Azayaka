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

#ifndef _CHANNEL2_HPP_
#define _CHANNEL2_HPP_

#include "channel.hpp"
#include "volume_envelope.hpp"

class Channel2 : public Channel
{
public:
    Channel2(GameBoy *gb);

    byte read(word address) override;
    void write(word address, byte value) override;

    void tick() override;
    void envelope_clock() override;
    void power_off() override;

    void save_state(State &state) override;
    void load_state(State &state) override;

private:
    void trigger();

    const bool duty_table[4][8] = {
        {0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 1, 0}
    };

    VolumeEnvelope volume_envelope;

    int timer;
    int sequence;
    int frequency;

    byte duty;
};

#endif // _CHANNEL2_HPP_
