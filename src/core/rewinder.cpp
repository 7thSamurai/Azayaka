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

#include "rewinder.hpp"
#include "gameboy.hpp"

#include <iostream>

// Based off https://binji.github.io/posts/binjgb-rewind/

RewindSeries::RewindSeries() {
    pos = 0;
}

bool RewindSeries::is_full() const {
    return pos == frames_per_key;
}

bool RewindSeries::is_empty() const {
    return pos == 0 && key_state.is_empty();
}

void RewindSeries::clear() {
    key_state.clear();

    for (CompressedState &state : compressed_states)
        state.clear();

    pos = 0;
}

void RewindSeries::push(GameBoy &gb) {
    if (key_state.is_empty())
        gb.save_state(key_state);

    else {
        State_Memory state;

        compressed_states[pos].clear();

        gb.save_state(state);
        compress(state);

        pos++;
    }
}

void RewindSeries::pop(GameBoy &gb) {
    if (pos == 0)
        gb.load_state(key_state);
    else {
        pos--;

        State_Memory state;

        decompress(state);
        gb.load_state(state);
    }
}

unsigned int RewindSeries::get_memory_used() const {
    unsigned int memory = key_state.size();

    for (const CompressedState &state : compressed_states)
        memory += state.size();

    return memory;
}

void RewindSeries::compress(State_Memory &state_in) {
    u8 *prev = &key_state.memory[0];
    u8 *data = &state_in .memory[0];
    u8 *end  = data + state_in.size();

    u8 last = *prev++ - *data++;
    u8 current;

    unsigned int count;

    compressed_states[pos].push_back(last);

    while (data != end) {
        current = *prev++ - *data++;

        if (current == last) {
            count = 0;

            while (data != end) {
                current = *prev++ - *data++;

                if (current != last)
                    break;

                count++;
            }

            compressed_states[pos].push_back(last);
            write_count(count);

            if (data == end)
                break;
        }

        compressed_states[pos].push_back(current);
        last = current;
    }
}

void RewindSeries::decompress(State_Memory &state_out) {
    u8 *prev = &key_state.memory[0];
    u8 *data = &compressed_states[pos][0];
    u8 *end  = data + compressed_states[pos].size();

    u8 last = *data++;
    u8 current;

    unsigned int count;

    state_out.memory.push_back(*prev++ - last);

    while (data != end) {
        current = *data++;

        if (current == last) {
            count = read_count(&data);

            for (; count > 0; count--)
                state_out.memory.push_back(*prev++ - current);
        }

        state_out.memory.push_back(*prev++ - current);

        last = current;
    }
}

void RewindSeries::write_count(unsigned int count) {
    u8 byte;

    do {
        byte = count & 0x7F;
        count >>= 7;

        if (count != 0)
            byte |= 0x80;

        compressed_states[pos].push_back(byte);
    } while(count != 0);
}

unsigned int RewindSeries::read_count(u8 **data) {
    unsigned int count = 0;
    unsigned int shift = 0;
    u8 byte;

    u8 *bytes = *data;

    while (1) {
        byte = *bytes++;
        *data += 1;

        count |= (byte & 0x7F) << shift;

        if ((byte & 0x80) == 0)
            break;

        shift += 7;
    }

    return count;
}


Rewinder::Rewinder() {
    rewind_series = nullptr;

    num_of_series = 0;
    pos      = 0;
    last_pos = 0;

    out_of_frames = 0;
}

Rewinder::~Rewinder() {
    if (rewind_series != nullptr)
        delete[] rewind_series;
}

void Rewinder::set_length(float seconds) {
    num_of_series = seconds * 60 / RewindSeries::frames_per_key;

    if (rewind_series != nullptr)
        delete[] rewind_series;

    rewind_series = new RewindSeries[num_of_series];

    pos      = 0;
    last_pos = 0;

    out_of_frames = 0;
}

void Rewinder::push(GameBoy &gb) {
    if (rewind_series[pos].is_full()) {
        if (++pos == num_of_series) // Wrap around
            pos = 0;

        if (rewind_series[pos].is_full())
            rewind_series[pos].clear();
    }

    rewind_series[pos].push(gb);
    last_pos = pos;

    out_of_frames = 0;
}

void Rewinder::pop(GameBoy &gb) {
    // Don't wind back too far
    if (pos == last_pos+1) {
        out_of_frames = 1;
        return;
    }

    if (rewind_series[pos].is_empty()) {
        if (pos == 0) { // Wrap-Around
            pos = num_of_series-1;
        }
        else
            pos--;

        // Don't wind back to uninitialized states
        if (rewind_series[num_of_series-1].is_empty())
            return;
    }

    rewind_series[pos].pop(gb);
}

unsigned int Rewinder::get_memory_used() const {
    unsigned int memory = 0;

    for (unsigned int i = 0; i < num_of_series; i++)
        memory += rewind_series[i].get_memory_used();

    return memory;
}

bool Rewinder::has_frames_left() {
    return !out_of_frames;
}

void Rewinder::clear() {
    for (unsigned int i = 0; i < num_of_series; i++)
        rewind_series[i].clear();

    pos = 0;
    last_pos = 0;

    out_of_frames = 0;
}
