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

#ifdef USE_PNG

#include "image.hpp"
#include <png.h>

namespace Common {

int save_png(const std::string &file_path, const Color *image, unsigned int width, unsigned int height) {
    FILE *file = fopen(file_path.c_str(), "wb");

    if (!file)
        return -1;

    png_bytep *row_pointers = new png_bytep[height];

    for (unsigned int y = 0; y < height; y++)
        row_pointers[y] = (png_bytep)&image[width * y];

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    setjmp(png_jmpbuf(png_ptr));
    png_init_io(png_ptr, file);

    setjmp(png_jmpbuf(png_ptr));
    png_set_IHDR(png_ptr, info_ptr, width, height,
                 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    setjmp(png_jmpbuf(png_ptr));
    png_write_image(png_ptr, row_pointers);

    setjmp(png_jmpbuf(png_ptr));
    png_write_end(png_ptr, nullptr);

    fclose(file);

    delete[] row_pointers;

    return 0;
}

}

#endif
