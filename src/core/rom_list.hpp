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

#ifndef _ROM_LIST_HPP_
#define _ROM_LIST_HPP_

#include <string>
#include <vector>
#include <filesystem>
#include <map>

class RomList
{
public:
    RomList();
    RomList(const std::string &directory);

    bool is_valid();

    std::string get_directory() const;
    void set_directory(const std::string &directory);

    void get_files_names(std::vector <std::string> &file_names);
    std::string get_file_path(const std::string &file_name);

    void update();

private:
    std::string directory;

    std::map <std::string, std::string> files;
};

#endif // _ROM_LIST_HPP_
