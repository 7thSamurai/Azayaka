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

#ifndef _HDMA_HPP_
#define _HDMA_HPP_

#include "component.hpp"

class State;

class Hdma : public Component
{
public:
    Hdma(GameBoy *gb);

    void tick();
    bool is_copying() const;

    byte read(word address) override;
    void write(word address, byte value) override;

    void save_state(State &state);
    void load_state(State &state);

    void set_hblank();

private:
    enum Mode {
        Mode_Gdma = 0,
        Mode_Hdma = 1
    };

    bool transfering, copying;
    Mode mode;

    word source;
    word dest;
    byte blocks;
};

#endif // _HDMA_HPP_
