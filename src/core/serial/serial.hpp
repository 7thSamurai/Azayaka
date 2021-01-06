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

#ifndef _SERIAL_HPP_
#define _SERIAL_HPP_

#include "component.hpp"
#include "serial_device.hpp"

class State;

class Serial : public Component
{
public:
    Serial(GameBoy *gb);
    ~Serial();

    void tick();

    byte read(word address) override;
    void write(word address, byte value) override;

    void save_state(State &state);
    void load_state(State &state);

    void set_serial_device(SerialDevice *serial_device);

private:
    void start_transfer();

    byte data;
    byte control;

    bool transfering;
    int length, count;
    int timer;

    SerialDevice *serial_device;
};

#endif // _SERIAL_HPP_
