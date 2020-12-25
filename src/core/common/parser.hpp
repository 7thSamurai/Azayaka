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

#ifndef _PARSER_HPP_
#define _PARSER_HPP_

#include <string>
#include <map>

class Option
{
public:
    Option(const std::string &long_option, char short_option, int num_of_args);
    virtual ~Option();

    std::string get_long_option() const;
    char get_short_option() const;
    int get_num_of_args() const;

    virtual std::string get_description() const = 0;

    virtual void set(char **argv, int index) = 0;

private:
    std::string long_option;
    char short_option;
    int num_of_args;
};

class Parser
{
public:
    void add_option(Option *option);

    int parse(int argc, char **argv, int &index);

private:
    int parse_long (int argc, char **argv, int &index);
    int parse_short(int argc, char **argv, int &index);

    std::map <std::string, Option*> long_options;
    std::map <char, Option*> short_options;
};

#endif // _PARSER_HPP_
