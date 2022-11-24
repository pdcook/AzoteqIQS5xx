#ifndef I2C_HELPERS_H
#define I2C_HELPERS_H

#include <Arduino.h>
#include <Wire.h>

#define END_COMM_REG 0xEEEE

class I2CHelpers
{
    public:
        static void intToTwoByteArray(int value, byte* bytes_array);
        static int byteArrayToInt(byte* bytes_array, int num_bytes);
        static byte readFromRegister(int device_address, int register_address, int bytes_to_read, byte* buf);
        static byte endCommunication(int device_address);
};


#endif // I2C_HELPERS_H
