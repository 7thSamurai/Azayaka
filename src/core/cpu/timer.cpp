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

#include "core/cpu/timer.hpp"
#include "core/defs.hpp"
#include "core/gameboy.hpp"
#include "core/cpu/cpu.hpp"
#include "core/state.hpp"
#include "common/logger.hpp"
#include "common/string_utils.hpp"

Timer::Timer(GameBoy *gb) : Component(gb) {
    div  = 0;
    tima = 0;
    tma  = 0;
    tac  = 0;

    timer_enabled = 0;
    current_bit   = 1 << bits[0];
    last_bit      = 0;

    overflow = 0;
    ticks_since_overflow = 0;
}

byte Timer::read(word address) {
    switch (address) {
        case 0xFF04:
            return div >> 8;

        case 0xFF05:
            return tima;

        case 0xFF06:
            return tma;

        case 0xFF07:
            return tac | 0xF8;

        default:
            break;
    }

    LOG_WARNING("Timer::read can't access address 0x" + StringUtils::hex(address));

    return 0;
}

void Timer::write(word address, byte value) {
    switch (address) {
        case 0xFF04:
            div = 0;
            return;

        case 0xFF05:
            // Writes to TIMA are ignored if written the same tick it is reloading
            if (ticks_since_overflow != 5) {
                tima = value;

                overflow = 0;
                ticks_since_overflow = 0;
            }
            return;

        case 0xFF06:
            tma = value;

            // If you write to TMA the same tick that TIMA is reloading, TIMA is also set with the new value
            if (ticks_since_overflow == 5)
                tima = value;
            return;

        case 0xFF07:
            {
                bool old_enabled = timer_enabled;
                word old_bit = current_bit;

                tac = value & 0b111;
                current_bit = 1 << bits[tac & 0b11];
                timer_enabled = (tac & BIT2) != 0;

                tima_glitch(old_enabled, old_bit);
            }
            return;

        default:
            break;
    }

    LOG_WARNING("Timer::write can't access address 0x" + StringUtils::hex(address));
}

void Timer::tick() {
    div++;

    bool bit = (div & current_bit) != 0;
    bit &= timer_enabled;

    // Detect falling-edge
    if (last_bit && !bit) {
        if (++tima == 0x00) {
            overflow = 1;
            ticks_since_overflow = 0;
        }
    }

    last_bit = bit;

    if (overflow) {
        ticks_since_overflow++;

        if (ticks_since_overflow == 4)
            gb->cpu->trigger_interrupt(INT50);

        else if (ticks_since_overflow == 5)
            tima = tma;

        else if (ticks_since_overflow == 6) {
            overflow = 0;
            ticks_since_overflow = 0;
        }
    }
}

void Timer::tima_glitch(bool old_enabled, word old_bit) {
    if (!old_enabled)
        return;

    if (div & old_bit) {
        if (!timer_enabled || !(div & current_bit)) {
            if (++tima == 0x00) {
                tima = tma;
                gb->cpu->trigger_interrupt(INT50);
            }

            last_bit = 0;
        }
    }
}

void Timer::save_state(State &state) {
    state.write16(div);
    state.write8(tima);
    state.write8(tma);
    state.write8(tac);

    state.write8(timer_enabled);
    state.write16(current_bit);
    state.write8(last_bit);

    state.write8(overflow);
    state.write32(ticks_since_overflow);
}

void Timer::load_state(State &state) {
    div  = state.read16();
    tima = state.read8();
    tma  = state.read8();
    tac  = state.read8();

    timer_enabled = state.read8();
    current_bit   = state.read16();
    last_bit      = state.read8();

    overflow = state.read8();
    ticks_since_overflow = state.read32();
}
