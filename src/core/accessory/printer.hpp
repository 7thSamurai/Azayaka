#ifndef _PRINTER_HPP_
#define _PRINTER_HPP_

#include "serial/serial_device.hpp"
#include "types.hpp"

#include <string>

class Printer : public SerialDevice
{
public:
    Printer();

    bool send();
    void receive(bool bit);

    void set_rom_path(const std::string &rom_path);

private:
    void byte_received(byte byte);
    void run_command(byte command);

    enum CommandPos {
        CommandPos_Magic1       = 0x00,
        CommandPos_Magic2       = 0x01,
        CommandPos_Id           = 0x02,
        CommandPos_Compression  = 0x03,
        CommandPos_LengthLow    = 0x04,
        CommandPos_LengthHigh   = 0x05,
        CommandPos_Data         = 0x06,
        CommandPos_ChecksumLow  = 0x07,
        CommandPos_ChecksumHigh = 0x08,
        CommandPos_KeepAlive    = 0x09,
        CommandPos_Status       = 0x0A
    };

    enum CommandId {
        CommandId_Init   = 0x01,
        CommandId_Start  = 0x02,
        CommandId_Data   = 0x04,
        CommandId_Status = 0x0F
    };

    CommandPos command_pos;
    CommandId command_id;

    byte byte_to_send;
    byte byte_being_received;

    int bits_received;

    int command_length;

    byte image[160*200];
    int image_offset;

    bool compression;
    word length_left;
    byte data[0x280];
    word checksum;
    byte status;

    std::string rom_path;
};

#endif // _PRINTER_HPP_
