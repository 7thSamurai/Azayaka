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

#include "sdl/opengl/display_gl.hpp"
#include "common/opengl/shader.hpp"
#include "common/opengl/texture.hpp"
#include "common/opengl/gl.hpp"

#include <SDL.h>
#include <iostream>

static const GLfloat vertices[] = {
    -1.0f,  1.0f,
    -1.0f, -1.0f,
     1.0f, -1.0f,

    -1.0f,  1.0f,
     1.0f, -1.0f,
     1.0f,  1.0f
};

static const GLfloat tex_coords[] = {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,

    0.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
};

DisplayGL::DisplayGL(SDL_Window *window) {
    this->window = window;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    texture = new Texture;
    texture->create();

    char *base_path = SDL_GetBasePath();

    shader = new Shader;
    shader->create(std::string(base_path)+"data/shaders/vertex.glsl", std::string(base_path)+"data/shaders/frag.glsl");

    SDL_free(base_path);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(tex_coords), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(tex_coords), tex_coords);
}

DisplayGL::~DisplayGL() {
    delete texture;
    delete shader;

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void DisplayGL::display(int index) {
    texture->update((const void*)video_buffer, display_width, display_height);

    glViewport(offset_x + (index*correct_width), offset_y, correct_width, correct_height);

    shader ->use();
    texture->use();

    shader->uniform1i("texture_sampler", 0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(vertices));

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void DisplayGL::clear() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void DisplayGL::show() {
    SDL_GL_SwapWindow(window);
}
