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

#include "common/image.hpp"
#include "common/file_utils.hpp"

namespace Common {

int save_image(const std::string &file_path, const Color *image, unsigned int width, unsigned int height) {
#ifdef USE_PNG
    return save_png(file_path + ".png", image, width, height);
#else
    return save_bmp(file_path + ".bmp", image, width, height);
#endif
}

int save_next_image(const std::string &file_path, const Color *image, unsigned int width, unsigned int height) {
#ifdef USE_PNG
    std::string path = File::get_next_path(File::remove_extension(file_path) + ".png");
    return save_png(path, image, width, height);
#else
    std::string path = File::get_next_path(File::remove_extension(file_path) + ".bmp");
    return save_bmp(path, image, width, height);
#endif
}

}
