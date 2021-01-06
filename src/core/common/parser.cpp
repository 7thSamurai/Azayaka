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

#include "parser.hpp"

Option::Option(const std::string &long_option, char short_option, int num_of_args) {
    this->long_option  = long_option;
    this->short_option = short_option;
    this->num_of_args  = num_of_args;
}

Option::~Option() {
}

std::string Option::get_long_option() const {
    return long_option;
}

char Option::get_short_option() const {
    return short_option;
}

int Option::get_num_of_args() const {
    return num_of_args;
}


void Parser::add_option(Option *option) {
    long_options [option->get_long_option ()] = option;

    if (option->get_short_option())
        short_options[option->get_short_option()] = option;
}

int Parser::parse(int argc, char **argv, int &index) {
    if (argv[index][0] == '-') {
        if (argv[index][1] == '-')
            return parse_long(argc, argv, index);

        else
            return parse_short(argc, argv, index);
    }

    return -1;
}

int Parser::parse_long(int argc, char **argv, int &index) {
    auto it = long_options.find((argv[index]+2));

    if (it == long_options.end())
        return -1;

    Option *option = it->second;

    if (index+option->get_num_of_args() >= argc)
        return -1;

    option->set(argv, index);
    index += option->get_num_of_args();

    return 0;
}

int Parser::parse_short(int argc, char **argv, int &index) {
    auto it = short_options.find(*(argv[index]+1));

    if (it == short_options.end())
        return -1;

    Option *option = it->second;

    if (index+option->get_num_of_args() >= argc)
        return -1;

    option->set(argv, index);
    index += option->get_num_of_args();

    return 0;
}
