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

#include "display.hpp"
#include "types.hpp"
#include "font.hpp"
#include "settings.hpp"

Display::Display() {
    video_buffer = new Color[160*144];

    status_time = 0;
    lock_aspect_ratio = 1;
}

Display::~Display() {
    delete[] video_buffer;
}

void Display::update(const Color *video_buffer) {
    std::copy(video_buffer, video_buffer + 160*144, this->video_buffer);

    if (status_time > 0) {
        draw_str(status_text, 0, 0, {0xFF, 0x00, 0x00, 0xFF});
        status_time--;
    }

    else if (permanent_status_text != "")
        draw_str(permanent_status_text, 0, 0, {0xFF, 0x00, 0x00, 0xFF});
}

void Display::resize(int w, int h) {
    if (lock_aspect_ratio) {
        float want_aspect = (float)display_width / (float)display_height;
        float real_aspect = (float)w / (float)h;

        float scale;

        if (want_aspect > real_aspect)
            scale = (float)w / display_width;
        else
            scale = (float)h / display_height;

        correct_width  = display_width  * scale;
        correct_height = display_height * scale;

        offset_x = (w - correct_width)  / 2;
        offset_y = (h - correct_height) / 2;
    }

    else {
        correct_width  = w;
        correct_height = h;

        offset_x = 0;
        offset_y = 0;
    }
}

void Display::set_status_text(const std::string &status_text, int seconds) {
    if (seconds == -1)
        permanent_status_text = status_text;

    else {
        this->status_text = status_text;
        status_time = seconds * 60;
    }
}

void Display::load_settings(Settings &settings) {
    lock_aspect_ratio = settings.video_lock_aspect_ratio;
}

void Display::clear_status_text() {
    permanent_status_text = "";
}

void Display::draw_char(char c, Color *buffer, const Color &color) {
    if (c < ' ' && c > '~')
        c = '?';

    const u8 *data = &font[(c - ' ') * FONT_WIDTH * FONT_HEIGHT];

    for (int y = 0; y < FONT_HEIGHT; y++) {
        for (int x = 0; x < FONT_WIDTH; x++) {
            if (*(data++))
                *buffer = color;

            buffer++;
        }

        buffer += display_width - FONT_WIDTH;
    }
}

void Display::draw_str(const std::string &str, int x, int y, const Color &color) {
    Color *buffer = video_buffer + y*display_width + x;

    int start_x = x;

    for (unsigned int i = 0; i < str.size(); i++) {
        if (x >= display_width) {
            x = start_x;
            y += FONT_HEIGHT;

            buffer = video_buffer + y*display_width + start_x;

            if (str[i] == ' ')
                continue;
        }

        if (y+FONT_HEIGHT >= display_height)
            break;

        draw_char(str[i], buffer, color);

        buffer += FONT_WIDTH;
        x      += FONT_WIDTH;
    }
}
