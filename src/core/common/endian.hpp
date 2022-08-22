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

#include "types.hpp"

#ifdef __linux__
    #include <endian.h>
    #define IS_BIG_ENDIAN (__BYTE_ORDER == __BIG_ENDIAN)
#else
    #define IS_BIG_ENDIAN 0
#endif

inline u16 little16(u16 n) {
    #if IS_BIG_ENDIAN == 1
        return (n << 8) | (n >> 8);
    #else
        return n;
    #endif
}

inline u32 little32(u32 n) {
    #if IS_BIG_ENDIAN == 1
        return ((n & 0xFF000000) >> 24) |
               ((n & 0x00FF0000) >> 8)  |
               ((n & 0x0000FF00) << 8)  |
               ((n & 0x000000FF) << 24);
    #else
        return n;
    #endif
}

inline u64 little64(u64 n) {
    #if IS_BIG_ENDIAN == 1
        u32 hi, lo;

        lo = n & 0xFFFFFFFF;
        n >>= 32;
        hi = n & 0xFFFFFFFF;

        n = little32(lo);
        n <<= 32;
        n |= little32(hi);
    #endif

    return n;
}
