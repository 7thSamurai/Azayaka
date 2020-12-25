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

#ifndef _FILE_UTILS_HPP_
#define _FILE_UTILS_HPP_

#include <string>
#include <vector>

namespace File {

std::string remove_extension(const std::string &file_name);
std::string get_extension(const std::string &file_name);

bool exists(const std::string &path);
bool is_directory(const std::string &path);

void list_files(const std::string &path, std::vector <std::string> &output);
void list_files_recursively(const std::string &path, std::vector <std::string> &output);

// Example:
//   You pass "image.bmp" to this function
//   In the given directory, there are files called "image - 1.bmp", "image - 2.bmp" and "image - 3.bmp"
//   Then the returned value would be "image - 4.bmp"
std::string get_next_path(const std::string &file_path);

}

#endif // _FILE_UTILS_HPP_
