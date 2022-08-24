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

#include "common/color.hpp"
#include <cassert>

Color::Color() : r(0x00), g(0x00), b(0x00), a(0xFF) {
}

Color::Color(std::uint8_t r0, std::uint8_t g0, std::uint8_t b0, std::uint8_t a0) : r(r0), g(g0), b(b0), a(a0) {
}

std::uint8_t &Color::operator [] (std::size_t i) {
    assert(i < 4);

    return reinterpret_cast<std::uint8_t*>(this)[i];
}

std::uint8_t Color::operator [] (std::size_t i) const {
    assert(i < 4);

    return reinterpret_cast<const std::uint8_t*>(this)[i];
}

bool Color::operator == (const Color &c) const {
    return r == c.r && g == c.g && b == c.b && a == c.a;
}

bool Color::operator != (const Color &c) const {
    return r != c.r || g != c.g || b != c.b || a != c.a;
}
