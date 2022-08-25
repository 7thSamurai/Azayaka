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

class Shader
{
public:
    Shader();
    Shader(const std::string &vertex_path, const std::string &frag_path);

    ~Shader();

    void create(const std::string &vertex_path, const std::string &frag_path);
    void use() const;

    void uniform1i(const std::string &name, int value);
    void uniform1f(const std::string &name, float value);

private:
    int create_shader(const std::string &file_path, int type);
    int find_uniform_location(const std::string &name);

    int vertex_shader, frag_shader;
    int program;
};
