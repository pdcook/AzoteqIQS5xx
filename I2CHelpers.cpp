#include <string>
#include "I2CHelpers.h"
#include <Wire.h>

void I2CHelpers::intToTwoByteArray(int value, byte* bytes_array)
{
    bytes_array[0] = (value >> 8) & 0xFF;
    bytes_array[1] = value & 0xFF;
}

int I2CHelpers::byteArrayToInt(byte* bytes_array, int num_bytes)
{
    int value = 0;
    for (int i = 0; i < num_bytes; i++)
    {
        value = (value << 8) + bytes_array[i];
    }
    return value;
}

int I2CHelpers::byteArrayToSignedInt(byte* bytes_array, int num_bytes)
{
    int value = 0;
    for (int i = 0; i < num_bytes; i++)
    {
        value = (value << 8) + bytes_array[i];
    }
    if (value > 32767)
    {
        value = value - 65536;
    }
    return value;
}

int I2CHelpers::wordToInt(byte high_byte, byte low_byte)
{
    return (high_byte << 8) + low_byte;
}

int I2CHelpers::wordToSignedInt(byte high_byte, byte low_byte)
{
    int value = (high_byte << 8) + low_byte;
    if (value > 32767)
    {
        value = value - 65536;
    }
    return value;
}


byte I2CHelpers::readFromCurrentAddress(int device_address, int bytes_to_read, byte* buf)
{
    // perform a read without specifying the register address
    // this will read from the current address if other read/writes
    // have been performed in the same communication window
    // or the default read address (stored in the register at 0x0675)
    // if no other read/write has been performed
    // this increases the speed of the read operation, by saving the
    // time required to specify the register address

    // request the bytes from the device, sending repeated start
    Wire.requestFrom(device_address, bytes_to_read, false);

    int i = 0;
    byte error = 0;
    while (Wire.available())
    {
      if (i >= bytes_to_read)
      {
        // if we have more bytes than we requested, return an error
        error = 6;
        break;
      }
      buf[i] = Wire.read();
      i++;
    }
    return error;
}

byte I2CHelpers::readFromRegister(int device_address, int register_address, int bytes_to_read, byte* buf)
{
    // start the transmission to device
    Wire.beginTransmission(device_address);

    // convert the register to read from to a byte array
    byte addrByteArray[2];
    I2CHelpers::intToTwoByteArray(register_address, addrByteArray);

    // send the register to read from
    Wire.write(addrByteArray, 2);

    // end the transmission WITHOUT releasing the bus (sending repeated start instead)
    byte error = Wire.endTransmission(false);

    // if there was an error, return it
    if (error != 0)
    {
        return error;
    }

    // request the bytes from the device, sending repeated start
    Wire.requestFrom(device_address, bytes_to_read, false);

    int i = 0;
    while (Wire.available())
    {
      if (i >= bytes_to_read)
      {
        // if we have more bytes than we requested, return an error
        error = 6;
        break;
      }
      buf[i] = Wire.read();
      i++;
    }
    return error;
}

byte I2CHelpers::writeToRegister(int device_address, int register_address, int bytes_to_write, byte* buf)
{
    // start the transmission to device
    Wire.beginTransmission(device_address);

    // convert the register to read from to a byte array
    byte addrByteArray[2];
    I2CHelpers::intToTwoByteArray(register_address, addrByteArray);

    // send the register to read from
    Wire.write(addrByteArray, 2);

    // send the bytes to write
    Wire.write(buf, bytes_to_write);

    byte error = Wire.endTransmission(true);

    return error;
}

bool I2CHelpers::getBit(byte b, int bit)
{
    return (b >> bit) & 1;
}

byte I2CHelpers::endCommunication(int device_address)
{
    //  End communication
    //
    //  As per the IQS550 datasheet:
    //
    //  Unlike the previous A000 implementation,
    //  an I2C STOP will not terminate the communication window.
    //  When all required I2C transactionshave been completed,
    //  the communication session must be terminated manually.
    //  This is achieved by sending the End Communication Window
    //  command, by writing a single byte (any data) to the address
    //  0xEEEE, followed by a STOP. This will end the communication
    //  window, RDY will go low and the IQS5xx will continue with
    //  a new sensing and processing cycle.

    Wire.beginTransmission(device_address); // start transmission with the TPS65

    // convert the register to read from to a byte array
    byte addrByteArray[2];
    I2CHelpers::intToTwoByteArray(END_COMM_REG, addrByteArray);

    Wire.write(addrByteArray, 2);   // send the End Communication Window command address
    Wire.write('a');                  // write one byte
    return Wire.endTransmission(true);     // end transmission WITH I2C STOP message
}
