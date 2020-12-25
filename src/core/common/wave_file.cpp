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

#include "wave_file.hpp"
#include "common/logger.hpp"
#include "globals.hpp"

WaveFile::WaveFile() {
    num_of_samples = 0;
}

WaveFile::~WaveFile() {
    close();
}

int WaveFile::open(const std::string &file_path, unsigned int rate) {
    if (!file.open(file_path, BinaryFile::Mode_Write)) {
        logger.log("Unable to open wave-file", Logger::Error);
        return -1;
    }
    else
        logger.log("Opened wave-file", Logger::Info, Logger::Verbose);

    // Write the header info
    write4("RIFF");           // Riff
    file.write32(0x00000000); // File size(Filled in later)
    write4("WAVE");           // File-type
    write4("fmt ");           // Format marker
    file.write32(0x00000010); // Format size
    file.write16(0x0001);     // Format(PCM)
    file.write16(0x0002);     // Number of channels
    file.write32(rate);       // Sample rate
    file.write32(rate*2*2);   // Bytes per second
    file.write16(0x0004);     // Bytes per sample
    file.write16(0x0010);     // Bits per sample
    write4("data");           // Data marker
    file.write32(0x00000000); // Data size(Filled in later)

    num_of_samples = 0;

    return 0;
}

void WaveFile::close() {
    if (!file.is_open())
        return;

    // File size
    file.seek(4, BinaryFile::Pos_Beg);
    file.write32(num_of_samples*4 + 36); // 44 - 8 = 36

    // Data size
    file.seek(40, BinaryFile::Pos_Beg);
    file.write32(num_of_samples*4);

    file.close();

    logger.log("Closed wave-file", Logger::Info, Logger::Verbose);
}

void WaveFile::write_sample(s16 left, s16 right) {
    // Write the sample
    file.write16(left);
    file.write16(right);

    num_of_samples++;
}

void WaveFile::write4(const char *str) {
    file.write(str, 4);
}
