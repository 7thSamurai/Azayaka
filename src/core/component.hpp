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

#ifndef _COMPONENT_HPP_
#define _COMPONENT_HPP_

#include "types.hpp"

class GameBoy;

class Component
{
public:
    Component(GameBoy *gb);
    virtual ~Component();

    virtual byte read(word address);
    virtual void write(word address, byte value);

    virtual byte read_instruction(word address);
    virtual byte read_operand    (word address);

protected:
    GameBoy *gb;
};

#endif // _COMPONENT_HPP_
