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

#ifndef _GPU_HPP_
#define _GPU_HPP_

#include "component.hpp"
#include "types.hpp"
#include "defs.hpp"
#include "dmg_palette.hpp"
#include "color_palette.hpp"
#include "lcdc.hpp"
#include "tile_attribute.hpp"
#include "sprite.hpp"

class State;

class Gpu : public Component
{
public:
    Gpu(GameBoy *gb);
    ~Gpu();

    bool needs_refresh() const;
    void clear_refresh();

    const Color *get_screen_buffer() const;

    byte read(word address) override;
    void write(word address, byte value) override;

    void write_oam (word address, byte value);
    void write_vram(word address, byte value);

    void write_lcdc(byte value);

    void tick();

    void save_state(State &state);
    void load_state(State &state);

private:
    enum Mode {
        Mode_HBlank = 0,
        Mode_VBlank = 1,
        Mode_Oam    = 2,
        Mode_Vram   = 3
    };

    unsigned int hblank_ticks() const;

    void check_lyc();
    void check_stat_intr(bool vblank_trigger=0);

    void render_background_scanline();
    void render_window_scanline();
    void render_sprite_scanline();

    inline void set_mode(Mode mode);

    bool can_get_vram() const;
    bool can_get_oam () const;
    bool can_get_color_palettes() const;

    Color *screen_buffer;
    bool refresh_screen;

	byte scan_line_row[160];
	bool scan_line_row_priority[160];

	Sprite *obj_scanline[10];

    Mode gpu_mode;

    unsigned int timer, off_clock;

    byte window_y, window_x;

    // GPU registers
    byte scroll_x;
    byte scroll_y;
    byte scan_line;
    byte ly_compare;
    byte lcdc_stat;
    bool stat_intr;

    byte vbk; // VRAM Bank

    byte *tile_set[2];

    byte *background_map;
    TileAttribute *tile_attributes;

    Lcdc lcdc;

    DmgPalette dmg_palette;
    DmgPalette dmg_sprite_palettes[2];

    ColorPalette color_palette;
    ColorPalette color_sprite_palette;

    Sprite sprites[40];
};

#endif // _GPU_HPP_
