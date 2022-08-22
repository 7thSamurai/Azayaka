// Copyright (C) 2020-2022 Zach Collins <the_7thSamurai@protonmail.com>
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

#pragma once

#include <vector>

#include "types.hpp"
#include "state.hpp"

class GameBoy;

typedef std::vector <u8> CompressedState;

class RewindSeries {
public:
    RewindSeries();

    bool is_full() const;
    bool is_empty() const;

    void clear();

    void push(GameBoy &gb);
    void pop (GameBoy &gb);

    unsigned int get_memory_used() const;

    static const unsigned int frames_per_key = 255; // TODO: Find a good value for this

private:
    void compress  (State_Memory &state_in);
    void decompress(State_Memory &state_out);

    // LEB128
    void write_count(unsigned int count);
    unsigned int read_count(u8 **data);

    State_Memory key_state;

    CompressedState compressed_states[frames_per_key];
    unsigned int pos;
};

class Rewinder
{
public:
    Rewinder();
    ~Rewinder();

    void set_length(float seconds);

    void push(GameBoy &gb);
    void pop (GameBoy &gb);

    unsigned int get_memory_used() const;

    bool has_frames_left();

    void clear();

private:
    RewindSeries *rewind_series;
    unsigned int num_of_series;
    unsigned int pos, last_pos;

    bool out_of_frames;
};
