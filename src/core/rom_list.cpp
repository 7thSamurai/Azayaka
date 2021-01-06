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

#include "rom_list.hpp"
#include "common/file_utils.hpp"

#include <filesystem>

RomList::RomList() {
    directory = "";
}

RomList::RomList(const std::string &directory) {
    this->directory = directory;
}

bool RomList::is_valid() {
    return File::is_directory(directory);
}

std::string RomList::get_directory() const {
    return directory;
}

void RomList::set_directory(const std::string &directory) {
    this->directory = directory;
}

void RomList::get_files_names(std::vector <std::string> &file_names) {
    file_names.clear();

    for (auto &file : files)
        file_names.push_back(file.first);
}

std::string RomList::get_file_path(const std::string &file_name) {
    return files.find(file_name)->second;
}

void RomList::update() {
    if (directory == "")
        return;

    files.clear();

    for (const auto &entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (!entry.is_regular_file())
            continue;

        if (entry.path().extension() == ".gb" || entry.path().extension() == ".gbc")
            files[entry.path().filename().string()] = entry.path().string();
    }
}
