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

#ifndef _DISPLAY_HPP_
#define _DISPLAY_HPP_

#include <string>

class Color;
class Settings;

class Display
{
public:
    Display();
    virtual ~Display();

    void update(const Color *video_buffer);
    virtual void display() = 0;

    void resize(int w, int h);

    // If seconds is -1 then show until you call "clear_status_text"
    void set_status_text(const std::string &status_text, int seconds);
    void clear_status_text();

    void load_settings(Settings &settings);

protected:
    void draw_char(char c, Color *buffer, const Color &color);
    void draw_str(const std::string &str, int x, int y, const Color &color);

    const int display_width  = 160;
    const int display_height = 144;

    int correct_width, correct_height;
    int offset_x, offset_y;
    bool lock_aspect_ratio;

    std::string status_text, permanent_status_text;
    int status_time;

    Color *video_buffer;
};

#endif // _DISPLAY_HPP_
