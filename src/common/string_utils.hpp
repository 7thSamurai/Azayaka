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

#include <string>

std::string hex(unsigned int n, unsigned int digits);

template<typename T>
std::string hex(const T &n) {
    // Automatically determine the number of digits required to represent the type
    return hex(n, sizeof(T) * 2);
}

int as_hex(char c);
int from_hex(const std::string &str);

bool is_a_num2 (const std::string &num);
bool is_a_num10(const std::string &num);
bool is_a_num16(const std::string &num);

std::string ftos(float f, int precision=2);

void to_upper(std::string &s);
