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

Serial::Serial(GameBoy *gb) : Component(gb) {
    data    = 0;
    control = 0;

    transfering = 0;
    timer = 0;
}

Serial::~Serial() {
}

void Serial::tick() {
    // TODO
}

byte Serial::read(word address) {
    if (address == 0xFF01)
        return data;

    else if (address == 0xFF02) {
        if (gb->gbc_mode)
            return control | 0x7C;
        else
            return control | 0x7E;
    }

    logger.log("Serial::read can't access address 0x" + hex(address, 4), Logger::Warning);

    return 0;
}

void Serial::write(word address, byte value) {
    if (address == 0xFF01)
        data = value;

    else if (address == 0xFF02) {
        control = value;

        if (control & BIT7)
            start_transfer();
    }

    else
        logger.log("Serial::write can't access address 0x" + hex(address, 4), Logger::Warning);
}

void Serial::start_transfer() {
    transfering = 1;

    if (control & BIT1)
        timer = 4096;
    else
        timer = 128;
}

void Serial::save_state(State &state) {
    state.write8(data);
    state.write8(control);

    state.write8(transfering);
    state.write32(timer);
}

void Serial::load_state(State &state) {
    data    = state.read8();
    control = state.read8();

    transfering = state.read8();
    timer = state.read32();
}
