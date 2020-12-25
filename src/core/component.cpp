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

#include "component.hpp"
#include "globals.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"

Component::Component(GameBoy *gb) {
    this->gb = gb;
}

Component::~Component() {
}

byte Component::read(word address) {
    logger.log("Component::read can't access address 0x" + hex(address, 4), Logger::Warning);

    return 0;
}

void Component::write(word address, byte value) {
    logger.log("Component::write can't access address 0x" + hex(address, 4), Logger::Warning);
}

byte Component::read_instruction(word address) {
    return read(address);
}

byte Component::read_operand(word address) {
    return read(address);
}

