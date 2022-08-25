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

#include "common/opengl/texture.hpp"
#include "common/opengl/gl.hpp"
#include "common/logger.hpp"

namespace GL {

Texture::Texture() : id(0), w(0), h(0) {
}

Texture::~Texture() {
    destroy();
}

void Texture::create() {
    LOG_DEBUG("Creating OpenGL texture...");

    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Texture::destroy() {
    if (id) {
        glDeleteTextures(1, &id);
        id = w = h = 0;
    }
}

void Texture::update(const Color *image, unsigned int width, unsigned int height) {
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
}

void Texture::use() const {
    glBindTexture(GL_TEXTURE_2D, id);
}

} // GL
