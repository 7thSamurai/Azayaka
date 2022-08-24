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

#include "core/gpu/lcdc.hpp"
#include "core/defs.hpp"
#include "core/state.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"

Lcdc::Lcdc() : Component(nullptr) {
    value = 0x00;
}

byte Lcdc::read(word address) {
    if (address == 0xFF40)
        return value;

    LOG_WARNING("Lcdc::read can't access address 0x" + hex(address));

    return 0;
}

void Lcdc::write(word address, byte value) {
    if (address == 0xFF40) {
        this->value = value;
        return;
    }

    LOG_WARNING("Lcdc::write can't access address 0x" + hex(address));
}

bool Lcdc::background_on() const {
    return (value & BIT0) != 0;
}

bool Lcdc::sprite_on() const {
    return (value & BIT1) != 0;
}

int Lcdc::sprite_size() const {
    return (value & BIT2) ? 16 : 8;
}

int Lcdc::background_map() const {
    return (value & BIT3) ? 0x0400 : 0x0000;
}

bool Lcdc::background_tile() const {
    return (value & BIT4) != 0;
}

bool Lcdc::window_on() const {
    return (value & BIT5) != 0;
}

int Lcdc::window_map() const {
    return (value & BIT6) ? 0x0400 : 0x0000;
}

bool Lcdc::lcd_on() const {
    return (value & BIT7) != 0;
}

void Lcdc::load_state(State &state) {
    value = state.read8();
}

void Lcdc::save_state(State &state) {
    state.write8(value);
}
