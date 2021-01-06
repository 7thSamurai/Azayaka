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

#include "binary_file.hpp"
#include "common/endian.hpp"

BinaryFile::BinaryFile() {
    file = nullptr;
}

BinaryFile::BinaryFile(const std::string &path, Mode mode) {
    open(path, mode);
}

BinaryFile::~BinaryFile() {
    close();
}

bool BinaryFile::open(const std::string &path, Mode mode) {
    switch (mode) {
        case Mode_Read:
            file = std::fopen(path.c_str(), "rb");
            break;

        case Mode_Write:
            file = std::fopen(path.c_str(), "wb");
            break;
    }

    return file != nullptr;
}

void BinaryFile::close() {
    if (is_open()) {
        std::fclose(file);
        file = nullptr;
    }
}

bool BinaryFile::is_open() const {
    return file != nullptr;
}

void BinaryFile::seek(int offset, Pos pos) {
    std::fseek(file, offset, pos);
}

unsigned int BinaryFile::tell() {
    return std::ftell(file);
}

unsigned int BinaryFile::size() {
    unsigned int old_pos = tell();
    seek(0, Pos_End);

    unsigned int new_pos = tell();
    seek(old_pos, Pos_Beg);

    return new_pos;
}


bool BinaryFile::read(void *data, unsigned int size) {
    return std::fread(data, 1, size, file) == size;
}

u8 BinaryFile::read8() {
    u8 value;
    read(&value, sizeof(value));

    return value;
}

u16 BinaryFile::read16() {
    u16 value;
    read(&value, sizeof(value));

    return little16(value);
}

u32 BinaryFile::read32() {
    u32 value;
    read(&value, sizeof(value));

    return little32(value);
}

u64 BinaryFile::read64() {
    u64 value;
    read(&value, sizeof(value));

    return little64(value);
}


bool BinaryFile::write(const void *data, unsigned int size) {
    return fwrite(data, 1, size, file) == size;
}

void BinaryFile::write8(u8 value) {
    write(&value, sizeof(value));
}

void BinaryFile::write16(u16 value) {
    value = little16(value);
    write(&value, sizeof(value));
}

void BinaryFile::write32(u32 value) {
    value = little32(value);
    write(&value, sizeof(value));
}

void BinaryFile::write64(u64 value) {
    value = little64(value);
    write(&value, sizeof(value));
}
