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

#include "core/component.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"

Component::Component(GameBoy *gb) {
    this->gb = gb;
}

Component::~Component() {
}

byte Component::read(word address) {
    LOG_WARNING("Component::read can't access address 0x" + hex(address, 4));

    return 0;
}

void Component::write(word address, byte value) {
    LOG_WARNING("Component::write can't access address 0x" + hex(address, 4));
}

byte Component::read_instruction(word address) {
    return read(address);
}

byte Component::read_operand(word address) {
    return read(address);
}
