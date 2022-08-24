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

#include "sdl/window_sdl.hpp"
#include "sdl/display_sdl.hpp"
#include "sdl/opengl/display_gl.hpp"
#include "core/display/display.hpp"
#include "common/logger.hpp"

#include <SDL.h>
#include <SDL_opengl.h>

WindowSDL::WindowSDL() {
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
        LOG_ERROR("Unable to initialize video subsystem");
    else
        LOG_DEBUG("Initialized video subsystem");

    window     = nullptr;
    gl_context = nullptr;

    display = nullptr;
}

WindowSDL::~WindowSDL() {
    if (window != nullptr)
        SDL_DestroyWindow(window);

    if (gl_context != nullptr)
        SDL_GL_DeleteContext(gl_context);

    if (display != nullptr)
        delete display;
}

int WindowSDL::create(const std::string &title, float scale, bool multiplayer, bool force_sdl) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    if (multiplayer)
        window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320*scale, 144*scale, SDL_WINDOW_OPENGL);
    else
        window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160*scale, 144*scale, SDL_WINDOW_OPENGL);

    if (window == NULL) {
        LOG_ERROR("Unable to create window: " + std::string(SDL_GetError()));
        return -1;
    }
    else
        LOG_DEBUG("Created window");

#ifdef USE_GL
    if (force_sdl) {
        gl_context = nullptr;
        LOG_DEBUG("Forcing SDL as the video-driver");
    }

    else {
        gl_context = SDL_GL_CreateContext(window);
        LOG_DEBUG("Created OpenGL context");

        // Get the OpenGL version
        int major = 0, minor = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        LOG_DEBUG("Got OpenGL "+std::to_string(major)+"."+std::to_string(minor));

        // Make sure the the OpenGL Version is new enough
        if (major*100 + minor < 303) {
            SDL_GL_DeleteContext(gl_context);
            gl_context = nullptr;

            LOG_NOTICE("Could not get new enough OpenGL(3.3 required), falling back to SDL video-driver");
        }
    }

    if (gl_context != nullptr) {
        SDL_GL_SetSwapInterval(0);
        display = new DisplayGL(window);
    }
    else
        display = new DisplaySDL(window);
#else
    LOG_DEBUG("Only SDL was enabled as the video-driver at compile-time");
    display = new DisplaySDL(window);
#endif

    SDL_SetWindowResizable(window, SDL_TRUE);
    SDL_SetWindowMinimumSize(window, multiplayer ? 320 : 160, 144);
    SDL_ShowCursor(0);

    if (multiplayer)
        display->resize(320*scale, 144*scale, 1);
    else
        display->resize(160*scale, 144*scale, 0);

    return 0;
}

void WindowSDL::update(const Color *buffer) {
    display->clear();

    display->update(buffer);
    display->display(0);

    display->show();
}

void WindowSDL::update(const Color *screen_buffer, int index) {
    if (index) {
        display->update(screen_buffer, false);
        display->display(1);
    }

    else {
        display->update(screen_buffer, true);
        display->display(0);
    }
}

void WindowSDL::clear() {
    display->clear();
}

void WindowSDL::show() {
    display->show();
}

void WindowSDL::set_title(const std::string &title) {
    SDL_SetWindowTitle(window, title.c_str());
}

void WindowSDL::resize(int w, int h, int num) {
    display->resize(w, h, num);
}

void WindowSDL::set_fullscreen(bool fullscreen) {
    if (fullscreen) {
        LOG_DEBUG("Switching to fullscreen");
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    else {
        LOG_DEBUG("Switching back from fullscreen");
        SDL_SetWindowFullscreen(window, 0);
    }
}

void WindowSDL::set_status_text(const std::string &status_text, int seconds) {
    display->set_status_text(status_text, seconds);
}

void WindowSDL::clear_status_text() {
    display->clear_status_text();
}

Display &WindowSDL::get_display() {
    return *display;
}
