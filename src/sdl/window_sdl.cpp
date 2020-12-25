// Copyright (C) 2020 Zach Collins
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

#include "window_sdl.hpp"
#include "display_sdl.hpp"
#include "opengl/display_gl.hpp"
#include "display/display.hpp"
#include "globals.hpp"
#include "common/logger.hpp"

#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

WindowSDL::WindowSDL() {
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
        logger.log("Unable to initialize video subsystem", Logger::Error);
    else
        logger.log("Initialized video subsystem", Logger::Info, Logger::Verbose);

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

int WindowSDL::create(const std::string &title, float scale, bool force_sdl) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160*scale, 144*scale, SDL_WINDOW_OPENGL);

    if (window == NULL) {
        logger.log("Unable to create window: " + std::string(SDL_GetError()), Logger::Error);
        return -1;
    }
    else
        logger.log("Created window", Logger::Info, Logger::Verbose);

#ifdef USE_GL
    if (force_sdl) {
        gl_context = nullptr;
        logger.log("Forcing SDL as the video-driver", Logger::Info, Logger::Verbose);
    }

    else {
        gl_context = SDL_GL_CreateContext(window);
        logger.log("Created OpenGL context", Logger::Info, Logger::Verbose);

        // Get the OpenGL version
        int major = 0, minor = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        logger.log("Got OpenGL "+std::to_string(major)+"."+std::to_string(minor), Logger::Info, Logger::Verbose);

        // Make sure the the OpenGL Version is new enough
        if (major*100 + minor < 303) {
            SDL_GL_DeleteContext(gl_context);
            gl_context = nullptr;

            logger.log("Could not get new enough OpenGL(3.3 required), falling back to SDL video-driver", Logger::Notice);
        }
    }

    if (gl_context != nullptr) {
        SDL_GL_SetSwapInterval(0);
        display = new DisplayGL(window);
    }
    else
        display = new DisplaySDL(window);
#else
    logger.log("Only SDL was enabled as the video-driver at compile-time", Logger::Info, Logger::Verbose);
    display = new DisplaySDL(window);
#endif

    SDL_SetWindowResizable(window, SDL_TRUE);
    SDL_SetWindowMinimumSize(window, 160, 144);
    SDL_ShowCursor(0);

    display->resize(160*scale, 144*scale);

    return 0;
}

void WindowSDL::update(const Color *screen_buffer) {
    display->update(screen_buffer);
    display->display();
}

void WindowSDL::set_title(const std::string &title) {
    SDL_SetWindowTitle(window, title.c_str());
}

void WindowSDL::resize(int w, int h) {
    display->resize(w, h);
}

void WindowSDL::set_fullscreen(bool fullscreen) {
    if (fullscreen) {
        logger.log("Switching to fullscreen", Logger::Info, Logger::Verbose);
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    else {
        logger.log("Switching back from fullscreen", Logger::Info, Logger::Verbose);
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
