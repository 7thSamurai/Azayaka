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

#include "serial.hpp"
#include "gameboy.hpp"
#include "state.hpp"
#include "cpu/cpu.hpp"
#include "globals.hpp"
#include "defs.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"

static SerialDeviceNull null_serial_device;

Serial::Serial(GameBoy *gb) : Component(gb) {
    data    = 0;
    control = 0;

    transfering = 0;
    length = 0;
    count  = 0;
    timer  = 0;

    serial_device = &null_serial_device;
}

Serial::~Serial() {
}

void Serial::tick() {
    if (!transfering || !length)
        return;

    if (--timer == 0) {
        timer = length;

        serial_device->receive(data & BIT7);
        data = (data << 1) | serial_device->send();

        if (++count == 8) {
            transfering = 0;
            length = 0;
            count  = 0;

            gb->cpu->trigger_interrupt(INT58);
        }
    }
}

byte Serial::read(word address) {
    if (address == 0xFF01)
        return data;

    else if (address == 0xFF02) {
        if (gb->gbc_mode)
            return control | (transfering ? BIT7 : 0) | 0x7C;
        else
            return control | (transfering ? BIT7 : 0) | 0x7E;
    }

    logger.log("Serial::read can't access address 0x" + hex(address, 4), Logger::Warning);

    return 0;
}

void Serial::write(word address, byte value) {
    if (address == 0xFF01)
        data = value;

    else if (address == 0xFF02) {
        control = value & 0b11;

        if (value & BIT7) {
            transfering = 1;

            if (control & BIT0)
                start_transfer();
            else
                length = 0;
        }
        else
            transfering = 0;
    }

    else
        logger.log("Serial::write can't access address 0x" + hex(address, 4), Logger::Warning);
}

void Serial::start_transfer() {
    length = gb->gbc_mode && (control & BIT1) ? 16 : 512;

    count = 0;
    timer = length;
}

void Serial::save_state(State &state) {
    state.write8(data);
    state.write8(control);

    state.write8(transfering);
    state.write32(length);
    state.write32(count);
    state.write32(timer);
}

void Serial::load_state(State &state) {
    data    = state.read8();
    control = state.read8();

    transfering = state.read8();
    length = state.read32();
    count  = state.read32();
    timer  = state.read32();
}

void Serial::set_serial_device(SerialDevice *serial_device) {
    this->serial_device = serial_device ? serial_device : &null_serial_device;
}
