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

class Color;

namespace GL {

class Texture
{
public:
    Texture();
    ~Texture();

    void create();
    void destroy();

    void update(const Color *image, unsigned int width, unsigned int height);
    void use() const;

    inline unsigned int width () const { return w; }
    inline unsigned int height() const { return h; }

private:
    unsigned int id;
    unsigned int w, h;
};

} // GL
