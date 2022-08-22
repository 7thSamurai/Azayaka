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

class Display;
class Color;

struct SDL_Window;

class WindowSDL
{
public:
    WindowSDL();
    ~WindowSDL();

    int create(const std::string &title, float scale, bool multiplayer, bool force_sdl);

    void update(const Color *buffer);
    void update(const Color *buffer, int index);

    void clear();
    void show();

    void set_title(const std::string &title);
    void resize(int w, int h, int num);

    void set_fullscreen(bool fullscreen);

    void set_status_text(const std::string &status_text, int seconds);
    void clear_status_text();

    Display &get_display();

protected:
    SDL_Window *window;
    void *gl_context;

    Display *display;
};
