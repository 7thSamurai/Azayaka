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

#include "core/serial/serial.hpp"
#include "core/gameboy.hpp"
#include "core/state.hpp"
#include "core/cpu/cpu.hpp"
#include "core/defs.hpp"
#include "common/logger.hpp"
#include "common/string_utils.hpp"

static SerialDeviceNull null_serial_device;

Serial::Serial(GameBoy *gb) : Component(gb) {
    data    = 0;
    control = 0;

    transfering = 0;
    length = 0;
    count  = 0;
    timer  = 0;

    internal_clock = 0;

    serial_device = &null_serial_device;
}

Serial::~Serial() {
}

void Serial::tick() {
    if (!internal_clock)
        return;
    else if (!transfering)
        return;

    if (--timer == 0) {
        timer = length;

        bool bit = serial_device->send();
        serial_device->receive(data & BIT7);
        data = (data << 1) | bit;

        check_transfer();
    }
}

bool Serial::send() {
    return internal_clock ? 1 : (data & BIT7);
}

void Serial::receive(bool bit) {
    if (!internal_clock) {
        data = (data << 1) | bit;
        check_transfer();
    }
}

byte Serial::read(word address) {
    if (address == 0xFF01)
        return data;

    else if (address == 0xFF02) {
        byte value = control | (internal_clock ? BIT0 : 0x00) | (transfering ? BIT7 : 0x00);

        return value | (gb->gbc_mode ? 0x7C : 0x7E);
    }

    LOG_WARNING("Serial::read can't access address 0x" + StringUtils::hex(address));

    return 0;
}

void Serial::write(word address, byte value) {
    if (address == 0xFF01)
        data = value;

    else if (address == 0xFF02) {
        control = value & 0b11;
        internal_clock = value & BIT0;

        if (value & BIT7) {
            transfering = 1;

            if (internal_clock)
                start_transfer();
        }
        else
            transfering = 0;
    }

    else
        LOG_WARNING("Serial::write can't access address 0x" + StringUtils::hex(address));
}

void Serial::start_transfer() {
    length = gb->gbc_mode && (control & BIT1) ? 16 : 512;

    count = 0;
    timer = length;
}

void Serial::check_transfer() {
    if (++count == 8) {
        transfering = 0;
        length = 0;
        count  = 0;

        gb->cpu->trigger_interrupt(INT58);
    }
}

void Serial::save_state(State &state) {
    state.write8(data);
    state.write8(control);

    state.write8(transfering);
    state.write32(length);
    state.write32(count);
    state.write32(timer);

    state.write8(internal_clock);
}

void Serial::load_state(State &state) {
    data    = state.read8();
    control = state.read8();

    transfering = state.read8();
    length = state.read32();
    count  = state.read32();
    timer  = state.read32();

    internal_clock = state.read8();
}

void Serial::set_serial_device(SerialDevice *serial_device) {
    this->serial_device = serial_device ? serial_device : &null_serial_device;
}
