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

#include "core/memory/dma.hpp"
#include "core/gameboy.hpp"
#include "core/memory/mmu.hpp"
#include "core/state.hpp"
#include "core/gpu/gpu.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"

Dma::Dma(GameBoy *gb) : Component(gb) {
    enabled    = 0;
    restarting = 0;

    timer  = 0;
    source = 0;
    value  = 0;
}

void Dma::tick() {
    if (!enabled)
        return;

    // FIXME: This should only write every 4 clocks
    if (++timer > 4) {
        restarting = 0;

        word offset = (timer - 4) >> 2;
        word current_source = source + offset;

        if (current_source > 0xe000)
            current_source &= ~0x2000;

        gb->gpu->write_oam(offset, gb->mmu->read_byte(current_source));

        if (timer > 160*4+4) {
            enabled = 0;
            timer = 0;
        }
    }
}

byte Dma::read(word address) {
    if (address == 0xFF46)
        return value;

    LOG_WARNING("Dma::read can't access address 0x" + hex(address, 4));

    return 0;
}

void Dma::write(word address, byte value) {
    if (address == 0xFF46) {
        this->value = value;

        source = value << 8;
        timer = 0;

        restarting = enabled;
        enabled = 1;
    }

    else
        LOG_WARNING("Dma::write can't access address 0x" + hex(address, 4));
}

bool Dma::is_transfering() const {
    return timer > 4 || restarting;
}

void Dma::save_state(State &state) {
    state.write8(enabled);
    state.write8(restarting);

    state.write32(timer);

    state.write16(source);
    state.write8(value);
}

void Dma::load_state(State &state) {
    enabled    = state.read8();
    restarting = state.read8();

    timer = state.read32();

    source = state.read16();
    value  = state.read8();
}
