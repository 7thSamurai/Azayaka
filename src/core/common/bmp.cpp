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

#include "image.hpp"
#include "common/binary_file.hpp"

namespace Common {

int save_bmp(const std::string &file_path, const Color *image, unsigned int width, unsigned int height) {
    BinaryFile file(file_path, BinaryFile::Mode_Write);

    if (!file.is_open())
        return -1;

    unsigned int image_size = width * height * sizeof(Color);
    unsigned int file_size = image_size + 54;

    // Write the header
    file.write8('B');         // ID
    file.write8('M');         // ID
    file.write32(file_size);  // Size of file
    file.write16(0x0000);     // Unused
    file.write16(0x0000);     // Unused
    file.write32(0x00000036); // Data offset
    file.write32(0x00000028); // Size of DIB header
    file.write32(width);      // Image width
    file.write32(-height);    // Image height
    file.write16(0x0001);     // Number of color planes
    file.write16(0x0020);     // Number of bits per pixel
    file.write32(0x00000000); // Compression type
    file.write32(image_size); // Size of image data
    file.write32(0x00000b13); // Vertical resolution
    file.write32(0x00000b13); // Horizontal resolution
    file.write32(0x00000000); // Number of colors in palette
    file.write32(0x00000000); // Number of important colors

    // Convert the pixels from RGBA to BGRA
    Color *data = new Color[width*height];
    for (unsigned int i = 0; i < width*height; i++)
        data[i] = {image[i].b, image[i].g, image[i].r, image[i].a};

    // Now actually write the image
    file.write(data, image_size);
    delete[] data;

    return 0;
}

}
