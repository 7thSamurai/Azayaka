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

#include "state.hpp"
#include "common/utils.hpp"

int State_File::open_read(const std::string &file_path) {
    return file.open(file_path, BinaryFile::Mode_Read) ? 0 : -1;
}

int State_File::open_write(const std::string &file_path) {
    return file.open(file_path, BinaryFile::Mode_Write) ? 0 : -1;
}

void State_File::close() {
    file.close();
}

void State_File::write8(u8 value) {
    file.write8(value);
}

void State_File::write16(u16 value) {
    file.write16(value);
}

void State_File::write32(u32 value) {
    file.write32(value);
}

void State_File::write_data(const void *data, unsigned int size) {
    file.write(data, size);
}

u8 State_File::read8() {
    return file.read8();
}

u16 State_File::read16() {
    return file.read16();
}

u32 State_File::read32() {
    return file.read32();
}

void State_File::read_data(void *data, unsigned int size) {
    file.read(data, size);
}


unsigned int State_Memory::size() const {
    return memory.size();
}

bool State_Memory::is_empty() const {
    return memory.empty();
}

void State_Memory::clear() {
    memory.clear();
}

void State_Memory::write8(u8 value) {
    memory.push_back(value);
}

void State_Memory::write16(u16 value) {
    memory.insert(memory.end(), (u8*)&value, (u8*)&value + sizeof(value));
}

void State_Memory::write32(u32 value) {
    memory.insert(memory.end(), (u8*)&value, (u8*)&value + sizeof(value));
}

void State_Memory::write_data(const void *data, unsigned int size) {
    memory.insert(memory.end(), (u8*)data, (u8*)data+size);
}

u8 State_Memory::read8() {
    u8 value = memory.front();
    memory.erase(memory.begin());

    return value;
}

u16 State_Memory::read16() {
    u16 value;

    std::copy(memory.begin(), memory.begin()+sizeof(value), (u8*)&value);
    memory.erase(memory.begin(), memory.begin()+sizeof(value));

    return value;
}

u32 State_Memory::read32() {
    u32 value;

    std::copy(memory.begin(), memory.begin()+sizeof(value), (u8*)&value);
    memory.erase(memory.begin(), memory.begin()+sizeof(value));

    return value;
}

void State_Memory::read_data(void *data, unsigned int size) {
    std::copy(memory.begin(), memory.begin()+size, (u8*)data);
    memory.erase(memory.begin(), memory.begin()+size);
}
