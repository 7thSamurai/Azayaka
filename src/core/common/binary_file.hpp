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

#include <cstdio>
#include <string>

class BinaryFile
{
public:
    enum Mode {
        Mode_Read,
        Mode_Write
    };

    enum Pos {
        Pos_Beg = 0,
        Pos_Cur = 1,
        Pos_End = 2
    };

    BinaryFile();
    BinaryFile(const std::string &path, Mode mode);

    ~BinaryFile();

    bool open(const std::string &path, Mode mode);
    void close();

    bool is_open() const;

    void seek(int offset, Pos pos);
    unsigned int tell();

    unsigned int size();

    bool read(void *data, unsigned int size);
    u8  read8();
    u16 read16();
    u32 read32();
    u64 read64();

    bool write(const void *data, unsigned int size);
    void write8 (u8  value);
    void write16(u16 value);
    void write32(u32 value);
    void write64(u64 value);

private:
    std::FILE *file;
};

#pragma once
