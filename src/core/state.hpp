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

#ifndef _STATE_HPP_
#define _STATE_HPP_

#include "types.hpp"
#include "common/binary_file.hpp"

#include <vector>
#include <string>

class State
{
public:
    virtual ~State() { }

    virtual void write8(u8 value)   = 0;
    virtual void write16(u16 value) = 0;
    virtual void write32(u32 value) = 0;

    virtual void write_data(const void *data, unsigned int size) = 0; // Size is in bytes

    virtual u8  read8 () = 0;
    virtual u16 read16() = 0;
    virtual u32 read32() = 0;

    virtual void read_data(void *data, unsigned int size) = 0; // Size is in bytes
};

class State_File : public State
{
public:
    int open_read (const std::string &file_path);
    int open_write(const std::string &file_path);
    void close();

    void write8(u8 value)   override;
    void write16(u16 value) override;
    void write32(u32 value) override;

    void write_data(const void *data, unsigned int size) override;;

    u8  read8 () override;
    u16 read16() override;
    u32 read32() override;

    void read_data(void *data, unsigned int size) override;

private:
    BinaryFile file;
};

class RewindSeries;

class State_Memory : public State
{
    friend RewindSeries;
public:
    unsigned int size() const;
    bool is_empty() const;
    void clear();

    void write8(u8 value)   override;
    void write16(u16 value) override;
    void write32(u32 value) override;

    void write_data(const void *data, unsigned int size) override;

    u8  read8 () override;
    u16 read16() override;
    u32 read32() override;

    void read_data(void *data, unsigned int size) override;

private:
    std::vector <u8> memory;
};

#endif // _STATE_HPP_
