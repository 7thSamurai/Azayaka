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

#include "sdl/display_sdl.hpp"
#include "core/globals.hpp"
#include "common/logger.hpp"

#include <SDL.h>

DisplaySDL::DisplaySDL(SDL_Window *window) {
    this->window = window;

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL)
        logger.log("Unable to create SDL Renderer: " + std::string(SDL_GetError()), Logger::Error);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, display_width, display_height);
    if (texture == NULL)
        logger.log("Unable to create SDL Texture: " + std::string(SDL_GetError()), Logger::Error);

    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

DisplaySDL::~DisplaySDL() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
}

void DisplaySDL::display(int index) {
    SDL_Rect rect = {offset_x + (index*correct_width), offset_y, correct_width, correct_height};
    SDL_RenderSetViewport(renderer, &rect);

    SDL_UpdateTexture(texture, NULL, video_buffer, display_width*4);

    SDL_RenderCopy(renderer, texture, NULL, NULL);
}

void DisplaySDL::clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void DisplaySDL::show() {
    SDL_RenderPresent(renderer);
}
