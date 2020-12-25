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

#ifndef _DEBUGGER_SDL_HPP_
#define _DEBUGGER_SDL_HPP_

#include "debug/debugger.hpp"

class Debugger_SDL : public Debugger
{
public:
    Debugger_SDL(GameBoy *gb);

private:
    void print(const std::string &msg) override;
};

#endif // _DEBUGGER_SDL_HPP_
