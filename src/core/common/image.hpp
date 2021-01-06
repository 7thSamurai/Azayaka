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

#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_

#include "types.hpp"
#include <string>

namespace Common {

// Saves a PNG or a BMP depending what you have when you compile
int save_image(const std::string &rom_path, const Color *image, unsigned int width, unsigned int height);

int save_bmp(const std::string &file_path, const Color *image, unsigned int width, unsigned int height);

#ifdef USE_PNG
int save_png(const std::string &file_path, const Color *image, unsigned int width, unsigned int height);
#endif

}

#endif // _IMAGE_HPP_
