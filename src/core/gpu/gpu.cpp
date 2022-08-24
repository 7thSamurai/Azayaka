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

#include "core/gpu/gpu.hpp"
#include "core/gameboy.hpp"
#include "core/cpu/cpu.hpp"
#include "core/memory/dma.hpp"
#include "core/memory/hdma.hpp"
#include "core/state.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"

Gpu::Gpu(GameBoy *gb) : Component(gb) {
    screen_buffer = new Color[160*144];
    refresh_screen = 0;

    tile_set[0] = new byte[0x1800];
    tile_set[1] = new byte[0x1800];

    background_map = new byte[0x800];
    tile_attributes = new TileAttribute[0x800];

    gpu_mode = Mode_HBlank;

    window_y = 0;
    window_x = 0;

    scroll_x   = 0;
    scroll_y   = 0;
    scan_line  = 0;
    ly_compare = 0;
    lcdc_stat  = 0;
    stat_intr  = 0;

    vbk = 0;

    timer     = 0;
    off_clock = 0;

    std::fill(tile_set[0], tile_set[0] + 0x1800, 0x00);
    std::fill(tile_set[1], tile_set[1] + 0x1800, 0x00);
    std::fill(background_map, background_map + 0x800, 0x00);

    std::fill(screen_buffer, screen_buffer + 160*144, Color{255, 255, 255, 255});
}

Gpu::~Gpu() {
    delete[] screen_buffer;

    delete[] tile_set[0];
    delete[] tile_set[1];

    delete[] background_map;
    delete[] tile_attributes;
}

bool Gpu::needs_refresh() const {
    return refresh_screen;
}

void Gpu::clear_refresh() {
    refresh_screen = 0;
}

const Color *Gpu::get_screen_buffer() const {
    return screen_buffer;
}

byte Gpu::read(word address) {
    if (address >= 0x8000 && address <= 0x97FF)
        return can_get_vram() ? tile_set[vbk][address - 0x8000] : 0xFF;

    else if (address >= 0x9800 && address <= 0x9FFF) {
        if (can_get_vram()) {
            if (vbk) return tile_attributes[address - 0x9800].read_byte();
            else     return background_map [address - 0x9800];
        }

        else
            return 0xFF;
    }

    else if (address >= 0xFE00 && address <= 0xFE9F) {
        address -= 0xFE00;

        if (can_get_oam() && !gb->dma->is_transfering())
            return sprites[address >> 2].read_byte(address);
        else
            return 0xFF;
    }

    switch (address) {
        case 0xFF40:
            return lcdc.read(0xFF40);
        case 0xFF41:
            return lcdc_stat | BIT7;
        case 0xFF42:
            return scroll_y;
        case 0xFF43:
            return scroll_x;
        case 0xFF44:
            return scan_line;
        case 0xFF45:
            return ly_compare;
        case 0xFF46:
            return 0xFF;
        case 0xFF47:
            return dmg_palette.get_palette();
        case 0xFF48:
            return dmg_sprite_palettes[0].get_palette();
        case 0xFF49:
            return dmg_sprite_palettes[1].get_palette();
        case 0xFF4A:
            return window_y;
        case 0xFF4B:
            return window_x;
        case 0xFF4F:
            return gb->gbc_mode ? (vbk | 0xFE) : 0xFF;
        case 0xFF68:
            return gb->gbc_mode ? color_palette.get_index() : 0xFF;
        case 0xFF69:
            return (gb->gbc_mode && can_get_color_palettes()) ? color_palette.get_palette() : 0xFF;
        case 0xFF6A:
            return gb->gbc_mode ? color_sprite_palette.get_index() : 0xFF;
        case 0xFF6B:
            return (gb->gbc_mode && can_get_color_palettes()) ? color_sprite_palette.get_palette() : 0xFF;

        default:
            break;
    }

    LOG_WARNING("Gpu::read can't access address: 0x" + hex(address, 4));

    return 0;
}

void Gpu::write(word address, byte value) {
    if (address >= 0x8000 && address <= 0x9FFF) {
        write_vram(address - 0x8000, value);
        return;
    }

    else if (address >= 0xFE00 && address <= 0xFE9F) {
        if (can_get_oam() && !gb->dma->is_transfering()) {
            address -= 0xFE00;

            sprites[address >> 2].write_byte(address, value);
        }

        return;
    }

    switch (address) {
        case 0xFF40:
            write_lcdc(value);
            return;
        case 0xFF41:
            lcdc_stat = value & 0xF8;

            if (lcdc.lcd_on())
                check_stat_intr();
            return;
        case 0xFF42:
            scroll_y = value;
            return;
        case 0xFF43:
            scroll_x = value;
            return;
        case 0xFF44:
            scan_line = 0;
            return;
        case 0xFF45:
            ly_compare = value;

            if (lcdc.lcd_on()) {
                check_lyc();
                check_stat_intr();
            }
            return;
        case 0xFF47:
            dmg_palette.set_palette(value);
            return;
        case 0xFF48:
            dmg_sprite_palettes[0].set_palette(value);
            return;
        case 0xFF49:
            dmg_sprite_palettes[1].set_palette(value);
            return;
        case 0xFF4A:
            window_y = value;
            return;
        case 0xFF4B:
            window_x = value;
            return;
        case 0xFF4F:
            vbk = value & BIT0;
            return;
        case 0xFF68:
            if (gb->gbc_mode)
                color_palette.set_index(value);
            return;
        case 0xFF69:
            if (gb->gbc_mode && can_get_color_palettes())
                color_palette.set_palette(value);
            return;
        case 0xFF6A:
            if (gb->gbc_mode)
                color_sprite_palette.set_index(value);
            return;
        case 0xFF6B:
            if (gb->gbc_mode && can_get_color_palettes())
                color_sprite_palette.set_palette(value);
            return;

        default:
            break;
    }

    LOG_WARNING("Gpu::write can't access address: 0x" + hex(address, 4));
}

void Gpu::write_oam(word address, byte value) {
    sprites[address >> 2].write_byte(address, value);
}

void Gpu::write_vram(word address, byte value) {
    if (!can_get_vram())
        return;

    if (address <= 0x17FF)
        tile_set[vbk][address] = value;

    else if (address <= 0x1FFF) {
        if (vbk) tile_attributes[address - 0x1800].write_byte(value);
        else     background_map [address - 0x1800] = value;
    }
}

void Gpu::write_lcdc(byte value) {
    bool lcd_was_on = lcdc.lcd_on();

    lcdc.write(0xFF40, value);

    // If the screen was turned off, then clear it.
    if (lcd_was_on && !lcdc.lcd_on()) {
        // The screen should not be turned off unless in VBLANK Mode
        if (gpu_mode != Mode_VBlank)
            LOG_WARNING("The screen shouldn't turn off while not in VBLANK");

        for (int i = 0; i < (160*144); i++)
            screen_buffer[i] = Color { 0xFF, 0xFF, 0xFF, 0xFF };

        set_mode(Mode_HBlank);

        scan_line = 0;
        off_clock = 0;
    }

    else if (!lcd_was_on && lcdc.lcd_on()) {
        check_lyc();
        check_stat_intr();

        timer = 0;
    }
}

void Gpu::tick() {
    if (!lcdc.lcd_on()) {
        off_clock++;

        // This is so if the Screen is off
        // The emulator isn't stuck in a loop
        // And can't exit
        if (off_clock >= 70224) {
            off_clock -= 70224;
            refresh_screen = 1;
        }

        return;
    }

    timer++;

    switch (gpu_mode) {
        case Mode_HBlank:
            if (gb->gbc_mode)
                gb->hdma->set_hblank();

            if (timer == hblank_ticks()) {
                timer = 0;
                scan_line++;

                check_lyc();

                if (scan_line == 144) {
                    set_mode(Mode_VBlank);
                    check_stat_intr(1);

                    gb->cpu->trigger_interrupt(INT40);
                    refresh_screen = 1;
                }

                else {
                    set_mode(Mode_Oam);
                    check_stat_intr();
                }
            }

            break;

        case Mode_VBlank:
            if (timer == 456) {
                timer = 0;
                scan_line++;

                check_lyc();
                check_stat_intr();

                if (scan_line == 154) {
                    set_mode(Mode_Oam);
                    scan_line = 0;

                    check_lyc();
                    check_stat_intr();
                }
            }

            break;

        case Mode_Oam:
            if (timer == 80) {
                timer = 0;
                set_mode(Mode_Vram);
            }

            break;

        case Mode_Vram:
            if (timer == 172) {
                timer = 0;
                set_mode(Mode_HBlank);

                check_stat_intr();

                render_background_scanline();
                render_window_scanline();
                render_sprite_scanline();
            }

            break;
    }
}

unsigned int Gpu::hblank_ticks() const {
    switch (scroll_x & 7) {
        case 0:       return 204;
        case 1 ... 4: return 200;
        case 5 ... 7: return 196;
    }

    return 0;
}

void Gpu::check_lyc() {
    if (ly_compare == scan_line)
        lcdc_stat |= BIT2;
    else
        lcdc_stat &= ~BIT2;
}

void Gpu::check_stat_intr(bool vblank_trigger) {
    if ((gpu_mode == Mode_HBlank && lcdc_stat & BIT3) ||
        (gpu_mode == Mode_VBlank && lcdc_stat & BIT4) ||
        (gpu_mode == Mode_Oam    && lcdc_stat & BIT5) ||
        (vblank_trigger          && lcdc_stat & BIT5) || // OAM enable also triggers VBLANK
        ((lcdc_stat & BIT6) && (lcdc_stat & BIT2)))
    {
        if (!stat_intr) {
            gb->cpu->trigger_interrupt(INT48);
            stat_intr = 1;
        }
    }

    else
        stat_intr = 0;
}

void Gpu::render_background_scanline() {
    Color *buffer = &screen_buffer[scan_line * 160];

    if (lcdc.background_on() || gb->gbc_mode) {
        word map_offset = lcdc.background_map();

        word Y = scan_line + scroll_y;

        word tile_y = ((Y >> 3) & 31) << 5;
        Y &= 7;

        word X, tile_map_address, tile_id;
        byte tile_x, pixel;

        bool background_tile = lcdc.background_tile();

        byte lo, hi;
        word tile_addr;

        // GBC stuff
        if (gb->gbc_mode) {
            // Render the scanline
            for (int x = 0; x < 160; x++, buffer++) {
                X = x + scroll_x;
                tile_x = (X >> 3) & 31;
                X &= 7;

                tile_map_address = map_offset + tile_y + tile_x;

                if (!background_tile)
                    tile_id = s8(background_map[tile_map_address]) + 256;
                else
                    tile_id = background_map[tile_map_address];

                // It is necessary to recalculate the Y position
                // every pass because Y-Flip may change it.
                Y = (scan_line + scroll_y) & 7;

                if (tile_attributes[tile_map_address].x_flip())
                    X = 7 - X;
                if (tile_attributes[tile_map_address].y_flip())
                    Y = 7 - Y;

                tile_addr = (tile_id << 4) + (Y << 1);
                lo = tile_set[tile_attributes[tile_map_address].vbank()][tile_addr+0] << X;
                hi = tile_set[tile_attributes[tile_map_address].vbank()][tile_addr+1] << X;
                pixel = (lo >> 7) | ((hi >> 6) & 0b10);

                *buffer = color_palette[tile_attributes[tile_map_address].pal_num()][pixel];

                scan_line_row_priority[x] = tile_attributes[tile_map_address].priority();
                scan_line_row[x] = pixel;
            }
        }

        else {
            // Render the scanline
            for (int x = 0; x < 160; x++, buffer++) {
                X = x + scroll_x;
                tile_x = (X >> 3) & 31;
                X &= 7;

                tile_map_address = map_offset + tile_y + tile_x;

                if (!background_tile)
                    tile_id = s8(background_map[tile_map_address]) + 256;
                else
                    tile_id = background_map[tile_map_address];

                tile_addr = (tile_id << 4) + (Y << 1);
                lo = tile_set[0][tile_addr+0] << X;
                hi = tile_set[0][tile_addr+1] << X;
                pixel = (lo >> 7) | ((hi >> 6) & 0b10);

                *buffer = dmg_palette[pixel];
                scan_line_row[x] = pixel;
            }
        }
    }
}

void Gpu::render_window_scanline() {
    // Check to see if the Window is enabled
    if (!lcdc.window_on())
        return;

    // Check if the window is off screen
    if (window_y > scan_line || window_x > 166)
        return;

    Color *buffer = &screen_buffer[scan_line * 160];

    // Get the Relative Window Position
    byte win_y = scan_line - window_y;

    // Find where the tile-map is
    word map_offset = lcdc.window_map();

    // Get the Y coordinate of the tile in 8 pixel size
    word tile_y = (win_y >> 3) << 5;

    // Get the X position of the window
    int win_x = window_x - 7;

    byte tile_x;
    word tile_map_address;
    word tile;
    byte pixel;
    byte X;
    byte Y = win_y & 7;

    word tile_addr;
    byte lo, hi;

    // Go through each pixel on the screen
    for (int x = 0; x < 160; x++, buffer++) {
        // Make sure the window isn't to the right
        if (x < win_x)
            continue;

        if ((x - win_x) < 0)
            continue;

        // Get the X coordinate of the tile in 8 pixel size
        tile_x = (x - win_x) >> 3;

        tile_map_address = map_offset + tile_y + tile_x;

        // Get the Tile-ID from the VRAM
        tile = background_map[tile_map_address];

        // If BG & Window tileset is enabled, then treat the
        // Tile-ID as signed number plus the offset
        if (!lcdc.background_tile())
            tile = 256 + s8(tile);

        // Get X coordinate of the current pixel in the tile
        X = (x - win_x) & 7;

        if (gb->gbc_mode) {
            Y = win_y & 7;

            if (tile_attributes[tile_map_address].x_flip())
                X = 7 - X;
            if (tile_attributes[tile_map_address].y_flip())
                Y = 7 - Y;

            tile_addr = (tile << 4) + (Y << 1);
            lo = tile_set[tile_attributes[tile_map_address].vbank()][tile_addr+0] << X;
            hi = tile_set[tile_attributes[tile_map_address].vbank()][tile_addr+1] << X;
            pixel = (lo >> 7) | ((hi >> 6) & 0b10);

            *buffer = color_palette[tile_attributes[tile_map_address].pal_num()][pixel];

            scan_line_row_priority[x] = tile_attributes[tile_map_address].priority();
        }

        else {
            tile_addr = (tile << 4) + (Y << 1);
            lo = tile_set[0][tile_addr+0] << X;
            hi = tile_set[0][tile_addr+1] << X;
            pixel = (lo >> 7) | ((hi >> 6) & 0b10);

            // Draw the pixel to the screen
            *buffer = dmg_palette[pixel];
        }

        scan_line_row[x] = pixel;
    }
}

void Gpu::render_sprite_scanline() {
    if (!lcdc.sprite_on())
        return;

    Color *buffer;
    Sprite *s;

    int height = lcdc.sprite_size();
    byte mask  = (height == 16) ? 0xFE : 0xFF;
    byte tile_row, color;

    int sprite_count = 0;

    for (int i = 0; i < 40; i++) {
        s = &sprites[i];

        if ((s->get_y() <= scan_line) && ((s->get_y() + height) > scan_line)) {
            int j = sprite_count;

            if (!gb->gbc_mode) {
                while (j > 0 && s->get_x() < obj_scanline[j-1]->get_x()) {
                    obj_scanline[j] = obj_scanline[j-1];
                    j--;
                }
            }

            obj_scanline[j] = s;

            if (++sprite_count == 10)
                break;
        }
    }

    int pixel_x;

    word tile_addr;
    byte hi, lo;
    byte X;

    for (int i = sprite_count-1; i >= 0; i--) {
        s = obj_scanline[i];
        buffer = &screen_buffer[scan_line*160 + s->get_x()];

        if (s->y_flip())
            tile_row = height - 1 - (scan_line - s->get_y());
        else
            tile_row = scan_line - s->get_y();

        for (int x = 0; x < 8; x++, buffer++) {
            pixel_x = s->get_x() + x;

            if (pixel_x < 0 || pixel_x >= 160)
                continue;

            if (gb->gbc_mode) {
                if (lcdc.background_on()) { // Master priority on CGB
                    if (scan_line_row_priority[pixel_x])
                        continue;

                    else if (s->priority() && scan_line_row[pixel_x])
                        continue;
                }
            }

            else if (s->priority() && scan_line_row[pixel_x])
                continue;

            X = s->x_flip() ? (7 - x) : x;

            if (gb->gbc_mode) {
                tile_addr = ((s->get_tile() & mask) << 4) + (tile_row << 1);
                lo = tile_set[s->color_vram()][tile_addr+0] << X;
                hi = tile_set[s->color_vram()][tile_addr+1] << X;
                color = (lo >> 7) | ((hi >> 6) & 0b10);

                if (color)
                    *buffer = color_sprite_palette[s->color_palette()][color];
            }

            else {
                tile_addr = ((s->get_tile() & mask) << 4) + (tile_row << 1);
                lo = tile_set[0][tile_addr+0] << X;
                hi = tile_set[0][tile_addr+1] << X;
                color = (lo >> 7) | ((hi >> 6) & 0b10);

                if (color)
                    *buffer = dmg_sprite_palettes[s->dmg_palette()][color];
            }
        }
    }
}

inline void Gpu::set_mode(Mode mode) {
    gpu_mode = mode;
    lcdc_stat = (lcdc_stat & 0xFC) | mode;
}

bool Gpu::can_get_vram() const {
    return gpu_mode != Mode_Vram;
}

bool Gpu::can_get_oam() const {
    return gpu_mode != Mode_Oam && gpu_mode != Mode_Vram;
}

bool Gpu::can_get_color_palettes() const {
    return gpu_mode != Mode_Vram;
}

void Gpu::save_state(State &state) {
    lcdc.save_state(state);

    dmg_palette.save_state(state);
    dmg_sprite_palettes[0].save_state(state);
    dmg_sprite_palettes[1].save_state(state);

    for (int i = 0; i < 40; i++)
        sprites[i].save_state(state);

    state.write_data(tile_set[0], 0x1800);
    state.write_data(background_map, 0x800);

    state.write8(gpu_mode);

    state.write32(timer);
    state.write32(off_clock);

    state.write8(window_y);
    state.write8(window_x);

    state.write8(scroll_x);
    state.write8(scroll_y);
    state.write8(scan_line);
    state.write8(ly_compare);
    state.write8(lcdc_stat);
    state.write8(stat_intr);

    if (gb->gbc_mode) {
        for (int i = 0; i < 0x800; i++)
            tile_attributes[i].save_state(state);

        color_palette.save_state(state);
        color_sprite_palette.save_state(state);

        state.write_data(tile_set[1], 0x1800);

        state.write8(vbk);
    }
}

void Gpu::load_state(State &state) {
    lcdc.load_state(state);

    dmg_palette.load_state(state);
    dmg_sprite_palettes[0].load_state(state);
    dmg_sprite_palettes[1].load_state(state);

    for (int i = 0; i < 40; i++)
        sprites[i].load_state(state);

    state.read_data(tile_set[0], 0x1800);
    state.read_data(background_map, 0x800);

    gpu_mode = (Mode)state.read8();

    timer     = state.read32();
    off_clock = state.read32();

    window_y   = state.read8();
    window_x   = state.read8();

    scroll_x   = state.read8();
    scroll_y   = state.read8();
    scan_line  = state.read8();
    ly_compare = state.read8();
    lcdc_stat  = state.read8();
    stat_intr  = state.read8();

    if (gb->gbc_mode) {
        for (int i = 0; i < 0x800; i++)
            tile_attributes[i].load_state(state);

        color_palette.load_state(state);
        color_sprite_palette.load_state(state);

        state.read_data(tile_set[1], 0x1800);

        vbk = state.read8();
    }
}
