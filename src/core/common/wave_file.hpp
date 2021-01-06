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

#ifndef _WAVE_FILE_HPP_
#define _WAVE_FILE_HPP_

#include "common/binary_file.hpp"

#include <string>

class WaveFile
{
public:
    WaveFile();
    ~WaveFile();

    int open(const std::string &file_path, unsigned int sample_rate);
    void close();

    void write_sample(s16 left, s16 right);

private:
    void write4(const char *str);

    BinaryFile file;
    unsigned int num_of_samples;
};

#endif // _WAVE_FILE_HPP_
