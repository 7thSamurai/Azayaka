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

#include "core/accessory/printer.hpp"
#include "core/defs.hpp"
#include "common/logger.hpp"
#include "common/utils.hpp"
#include "common/image.hpp"
#include "common/file_utils.hpp"

Printer::Printer() {
    byte_to_send = 0;
    byte_being_received = 0;

    bits_received = 0;
}

bool Printer::send() {
    bool bit = byte_to_send & BIT7;
    byte_to_send <<= 1;

    return bit;
}

void Printer::receive(bool bit) {
    byte_being_received <<= 1;
    byte_being_received |= bit;

    if (++bits_received == 8) {
        byte_received(byte_being_received);

        byte_being_received = 0;
        bits_received = 0;
    }
}

void Printer::set_rom_path(const std::string &rom_path) {
    this->rom_path = File::remove_extension(rom_path);
}

void Printer::byte_received(byte byte) {
    byte_to_send = 0;

    switch (command_pos) {
        case CommandPos_Magic1:
            if (byte != 0x88)
                return;

            command_length = 0;
            break;

        case CommandPos_Magic2:
            if (byte != 0x33) {
                if (byte != 0x88)
                    command_pos = CommandPos_Magic1;
                return;
            }
            break;

        case CommandPos_Id:
            command_id = (CommandId)byte;
            break;

        case CommandPos_Compression:
            compression = byte & BIT0;

            if (compression)
                LOG_ERROR("Printer is using compressed data, currently unsupported!");
            break;

        case CommandPos_LengthLow:
            length_left = byte;
            break;

        case CommandPos_LengthHigh:
            length_left |= byte << 8;
            break;

        case CommandPos_Data:
            data[command_length++] = byte;
            length_left--;
            break;

        case CommandPos_ChecksumLow:
            checksum = byte;
            break;

        case CommandPos_ChecksumHigh:
            checksum |= byte << 8;

            byte_to_send = 0x81;
            break;

        case CommandPos_KeepAlive:
            break;

        case CommandPos_Status:
            run_command(command_id);

            command_pos = CommandPos_Magic1;
            return;

        default:
            break;
    }

    if (command_pos != CommandPos_Data)
        command_pos = CommandPos(command_pos+1);

    if (command_pos == CommandPos_Data) {
        if (!length_left)
            command_pos = CommandPos(command_pos+1);
    }
}

void Printer::run_command(byte command) {
    switch (command) {
        case CommandId_Init:
            status = 0x00;
            image_offset = 0;
            std::fill(image, image + 160*200, 0x00);
            break;

        case CommandId_Start:
            status = 0x06; // Printing

            {
                Color image[image_offset];
                byte palette = data[2];

                const Color colors[4] = {
                    {0xFF, 0xFF, 0xFF, 0xFF},
                    {0xAA, 0xAA, 0xAA, 0xFF},
                    {0x55, 0x55, 0x55, 0xFF},
                    {0x00, 0x00, 0x00, 0xFF}
                };

                for (int i = 0; i < image_offset; i++)
                    image[i] = colors[(palette >> (this->image[i] << 1)) & 0b11];

                Common::save_next_image(rom_path + " - printer.bmp", image, 160, image_offset/160);
            }
            break;

        case CommandId_Data:
            if (command_length == 0x280) {
                status = 0x08; // Ready to print

                byte *byte = data;

                for (int row = 0; row < 2; row++) {
                    for (int col = 0; col < 20; col++) {
                        for (int y = 0; y < 8; y++, byte += 2) {
                            for (int x = 0; x < 8; x++) {
                                image[image_offset + col*8 + y*160 + x] = ((*byte) >> 7) | (((*(byte+1)) >> 6) & 0b10);

                                *(byte+0) <<= 1;
                                *(byte+1) <<= 1;
                            }
                        }
                    }

                    image_offset += 160 * 8;
                }
            }

            // Ignore empty packets
            else if (command_length != 0)
                LOG_ERROR("Printer gave unsupported data size of 0x" + hex(command_length, 4));
            break;

        case CommandId_Status:
            byte_to_send = status;

            if (status == 0x06) // Printing...
                status = 0x04;  // Printing done
            break;

        default:
            LOG_WARNING("Unknown printer command 0x" + hex(command, 2));
            break;
    }
}
