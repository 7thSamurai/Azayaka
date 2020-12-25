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

#include "file_utils.hpp"

#include <filesystem>
#include <iostream>

namespace File {

std::string remove_extension(const std::string &file_name) {
    return std::filesystem::path(file_name).replace_extension();
}

std::string get_extension(const std::string &file_name) {
    return std::filesystem::path(file_name).extension();
}

bool exists(const std::string &path) {
    return std::filesystem::exists(std::filesystem::path(path));
}

bool is_directory(const std::string &path) {
    return std::filesystem::is_directory(std::filesystem::path(path));
}

void list_files(const std::string &path, std::vector <std::string> &output) {
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        if (!entry.is_regular_file())
            continue;

        output.push_back(entry.path().string());
    }
}

void list_files_recursively(const std::string &path, std::vector <std::string> &output) {
    for (const auto &entry : std::filesystem::recursive_directory_iterator(path)) {
        if (!entry.is_regular_file())
            continue;

        output.push_back(entry.path().string());
    }
}

std::string get_next_path(const std::string &file_path) {
    std::string dir = std::filesystem::path(file_path).parent_path();
    std::string name = remove_extension(file_path);
    std::string extension = get_extension(file_path);

    std::vector <std::string> files;
    list_files(dir, files);

    unsigned int highest = 0;
    std::string ext;

    for (std::string &path : files) {
        ext  = get_extension(path);
        path = remove_extension(path);

        if (path.substr(0, path.find_last_of('-')-1) == name && ext == extension) {
            unsigned int num = std::stoi(path.substr(name.size()+3, std::string::npos));

            if (num > highest)
                highest = num;
        }
    }

    return name + " - " + std::to_string(highest+1) + extension;
}

}
