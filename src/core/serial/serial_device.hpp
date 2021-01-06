#ifndef _SERIAL_DEVICE_HPP_
#define _SERIAL_DEVICE_HPP_

class SerialDevice
{
public:
    virtual ~SerialDevice() { }

    virtual bool send() = 0;
    virtual void receive(bool bit) = 0;
};

class SerialDeviceNull : public SerialDevice
{
public:
    bool send() { return 1; }
    void receive(bool bit) { }
};

#endif // _SERIAL_DEVICE_HPP_
