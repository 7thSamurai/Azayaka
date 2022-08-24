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

#include "core/memory/hdma.hpp"
#include "core/gameboy.hpp"
#include "core/state.hpp"
#include "core/memory/mmu.hpp"
#include "core/gpu/gpu.hpp"
#include "core/defs.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"

Hdma::Hdma(GameBoy *gb) : Component(gb) {
    mode = Mode_Gdma;

    transfering = 0;
    copying     = 0;

    source = 0x0000;
    dest   = 0x0000;

    blocks = 0x01;
}

void Hdma::tick() {
    gb->gpu->write_vram(dest++ & 0x1FFF, gb->mmu->read_byte(source++));

    // If a block has finished
    if ((dest & 0xF) == 0) {
        if (--blocks == 0) {
            transfering = 0;
            copying     = 0;
            blocks = 0x80; // (0x80 - 1) = 0x7F
        }

        if (mode == Mode_Hdma)
            copying = 0;
    }
}

bool Hdma::is_copying() const {
    return copying;
}

byte Hdma::read(word address) {
    if (!gb->gbc_mode)
        return 0xFF;

    switch (address) {
        case 0xFF51:
        case 0xFF52:
        case 0xFF53:
        case 0xFF54:
            return 0xFF;

        case 0xFF55:
            return (transfering ? 0 : BIT7) | (blocks - 1);

        default:
            break;
    }

    LOG_WARNING("Hdma::read can't access address 0x" + hex(address));

    return 0;
}

void Hdma::write(word address, byte value) {
    if (!gb->gbc_mode)
        return;

    switch (address) {
        case 0xFF51:
            source = (source & 0x00FF) | (value << 8);
            return;

        case 0xFF52:
            source = (source & 0xFF00) | (value & 0xF0);
            return;

        case 0xFF53:
            dest = (dest & 0x00FF) | ((value & 0x1F) << 8);
            return;

        case 0xFF54:
            dest = (dest & 0xFF00) | (value & 0xF0);
            return;

        case 0xFF55:
            // If HDMA copy is active
            if (mode == Mode_Hdma && transfering) {
                if (Mode(value >> 7) == Mode_Gdma)
                    transfering = 0; // Stop copy
                else {
                    // Restart copy
                    mode = Mode(value >> 7);
                    blocks = (value & 0x7F) + 1;
                }
            }
            else {
                // Start copy
                mode = Mode(value >> 7);
                blocks = (value & 0x7F) + 1;

                transfering = 1;
            }

            // Start GDMA transfer immediately
            if (mode == Mode_Gdma)
                copying = 1;
            return;

        default:
            break;
    }

    LOG_WARNING("Hdma::write can't access address 0x" + hex(address));
}

void Hdma::save_state(State &state) {
    if (gb->gbc_mode) {
        state.write8(transfering);
        state.write8(copying);
        state.write8(mode);

        state.write16(source);
        state.write16(dest);

        state.write8(blocks);
    }
}

void Hdma::load_state(State &state) {
    if (gb->gbc_mode) {
        transfering = state.read8();
        copying     = state.read8();
        mode        = (Mode)state.read8();

        source = state.read16();
        dest   = state.read16();

        blocks = state.read8();
    }
}

void Hdma::set_hblank() {
    if (gb->gbc_mode && transfering && mode == Mode_Hdma)
        copying = 1;
}
