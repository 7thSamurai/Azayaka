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

#include "common/ini_file.hpp"

#include <fstream>

IniFile::Section::Section(const std::string &name) {
    this->name = name;
}

std::string IniFile::Section::get_str(const std::string &key) const {
    auto it = values.find(key);

    if (it != values.end())
        return it->second;

    return "";
}

int IniFile::Section::get_int(const std::string &key) const {
    return std::stoi(get_str(key));
}

bool IniFile::Section::get_bool(const std::string &key) const {
    return get_str(key) == "True";
}

void IniFile::Section::set_str(const std::string &key, const std::string &value) {
    values[key] = value;
}

void IniFile::Section::set_int(const std::string &key, int value) {
    values[key] = std::to_string(value);
}

void IniFile::Section::set_bool(const std::string &key, bool value) {
    values[key] = value ? "True" : "False";
}


int IniFile::load(const std::string &file_path) {
    sections.clear();

    std::ifstream file(file_path);

    if (!file.is_open())
        return -1;

    std::string line;

    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        if (line.front() == '[' && line.back() == ']')
            sections.push_back(line.substr(1, line.size()-2));

        else if (!sections.empty())
            parse_line(line);
    }

    file.close();

    return 0;
}

int IniFile::save(const std::string &file_path) {
    std::ofstream file(file_path);

    if (!file.is_open())
        return -1;

    for (const Section &section : sections) {
        file << '[' << section.name << ']' << std::endl;

        for (auto it : section.values)
            file << it.first << '=' << it.second << std::endl;

        file << std::endl;
    }

    file.close();

    return 0;
}

IniFile::Section *IniFile::get_section(const std::string &name) {
    for (Section &section : sections) {
        if (section.name == name)
            return &section;
    }

    return nullptr;
}

IniFile::Section *IniFile::get_or_create_section(const std::string &name) {
    for (Section &section : sections) {
        if (section.name == name)
            return &section;
    }

    sections.push_back(Section(name));
    return &sections.back();
}

void IniFile::parse_line(const std::string &line) {
    // Skip comments
    if (line.front() == ';' || line.front() == '#')
        return;

    unsigned int pos = line.find_first_of('=');

    if (pos != std::string::npos)
        sections.back().set_str(line.substr(0, pos), line.substr(pos+1, std::string::npos));
}
