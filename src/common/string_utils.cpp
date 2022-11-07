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

#include "common/string_utils.hpp"

#include <sstream>
#include <iomanip>
#include <algorithm>

namespace StringUtils {

std::string hex(unsigned int n, unsigned int digits) {
    std::stringstream ss;

    ss << std::setfill('0') << std::setw(digits) << std::hex << n;

    return ss.str();
}

int as_hex(char c) {
    return c >= 'A' ? c - 'A' + 10 : c - '0';
}

int from_hex(const std::string &str) {
    std::string h = str;
    to_upper(h);

    if (h[0] == '$')
        return std::stoi(h.substr(1), 0, 16);

    else if (h[0] == '0' && h[1] == 'X')
        return std::stoi(h.substr(2), 0, 16);

    return -1;
}

bool is_a_num2(const std::string &num) {
    for (char c : num) {
        if (c != '0' || c != '1')
            return 0;
    }

    return 1;
}

bool is_a_num10(const std::string &num) {
    for (char c : num) {
        if (!std::isdigit(c))
            return 0;
    }

    return 1;
}

bool is_a_num16(const std::string &num) {
    for (char c : num) {
        if (!std::isxdigit(c))
            return 0;
    }

    return 1;
}

std::string ftos(float f, int precision) {
    std::stringstream ss;

    ss << std::fixed << std::setprecision(precision) << f;

    return ss.str();
}

std::string to_lower(const std::string &s) {
    // Convert to lowercase
    auto copy = s;
    std::transform(copy.begin(), copy.end(), copy.begin(), ::tolower);

    return copy;
}

std::string to_upper(const std::string &s) {
    // Convert to lowercase
    auto copy = s;
    std::transform(copy.begin(), copy.end(), copy.begin(), ::toupper);

    return copy;
}

} // StringUtils
