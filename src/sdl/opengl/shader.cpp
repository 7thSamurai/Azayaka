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

#include "shader.hpp"
#include "gl.hpp"
#include "globals.hpp"
#include "common/logger.hpp"

#include <fstream>
#include <sstream>

Shader::Shader() {
    vertex_shader = 0;
    frag_shader   = 0;
    program       = 0;
}

Shader::Shader(const std::string &vertex_path, const std::string &frag_path) {
    create(vertex_path, frag_path);
}

Shader::~Shader() {
    logger.log("Deleting OpenGL stuff", Logger::Info, Logger::Verbose);

    if (program) {
        glDetachShader(program, vertex_shader);
        glDetachShader(program, frag_shader);
        glDeleteProgram(program);
    }

    if (vertex_shader) glDeleteShader(vertex_shader);
    if (frag_shader)   glDeleteShader(frag_shader);
}

void Shader::create(const std::string &vertex_path, const std::string &frag_path) {
    vertex_shader = create_shader(vertex_path, GL_VERTEX_SHADER);
    frag_shader   = create_shader(frag_path, GL_FRAGMENT_SHADER);

    program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, frag_shader);

    glLinkProgram(program);

    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
        logger.log("Unable to link shader-program", Logger::Error);
    else
        logger.log("Successfully linked shader-program", Logger::Info, Logger::Verbose);
}

void Shader::use() const {
    glUseProgram(program);
}

void Shader::uniform1i(const std::string &name, int value) {
    glUniform1i(find_uniform_location(name), value);
}

void Shader::uniform1f(const std::string &name, float value) {
    glUniform1f(find_uniform_location(name), value);
}

int Shader::create_shader(const std::string &file_path, int type) {
    std::ifstream file(file_path);

    if (!file.is_open())
        logger.log("Unable to open shader source-file: " + file_path, Logger::Error);
    else
        logger.log("Opened shader source-file: " + file_path, Logger::Info, Logger::Verbose);

    std::stringstream ss;
    ss << file.rdbuf();
    std::string source = ss.str();

    file.close();

    const char *source_pointer = source.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source_pointer, NULL);
    glCompileShader(shader);

    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
        logger.log("Unable to compile shader: " + file_path, Logger::Error);
    else
        logger.log("Successfully compiled shader: " + file_path, Logger::Info, Logger::Verbose);

    return shader;
}

int Shader::find_uniform_location(const std::string &name) {
    return glGetUniformLocation(program, name.c_str());
}
