// Copyright (C) 2020-2022 Zach Collins <the_7thSamurai@protonmail.com>
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

#pragma once

#include <cstdint>

class Color
{
public:
    Color();
    Color(std::uint8_t r0, std::uint8_t g0, std::uint8_t b0, std::uint8_t a0 = 0xFF);

    std::uint8_t &operator [] (std::size_t i);
    std::uint8_t  operator [] (std::size_t i) const;

    bool operator == (const Color &c) const;
    bool operator != (const Color &c) const;

    std::uint8_t r, g, b, a;
};
