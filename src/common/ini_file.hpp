// Copyright (C) 2020-2022 Zach Collins <the_7thSamurai@protonmail.com>
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

#pragma once

#include <string>
#include <map>
#include <list>

class IniFile
{
public:
    class Section {
        friend IniFile;
    public:
        Section(const std::string &name);

        std::string get_str(const std::string &key) const;
        int get_int(const std::string &key) const;
        bool get_bool(const std::string &key) const;

        void set_str(const std::string &key, const std::string &value);
        void set_int(const std::string &key, int value);
        void set_bool(const std::string &key, bool value);

    private:
        std::string name;
        std::map <std::string, std::string> values;
    };

    int load(const std::string &file_path);
    int save(const std::string &file_path);

    Section *get_section(const std::string &name);
    Section *get_or_create_section(const std::string &name);

private:
    void parse_line(const std::string &line);

    std::list <Section> sections;
};
