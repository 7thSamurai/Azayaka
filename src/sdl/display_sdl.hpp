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

#ifndef _RENDERER_HPP_
#define _RENDERER_HPP_

#include "display/display.hpp"
#include "window_sdl.hpp"

#include <cstdint>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

class DisplaySDL : public Display
{
public:
    DisplaySDL(SDL_Window *window);
    ~DisplaySDL();

    void display(int index = 0) override;
    void clear() override;
    void show () override;

private:
    SDL_Window   *window;
    SDL_Renderer *renderer;
    SDL_Texture  *texture;
};

#endif // _RENDERER_HPP_
