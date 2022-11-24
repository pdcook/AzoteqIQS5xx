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
    Wire.write(0);                  // write one byte
    return Wire.endTransmission(true);     // end transmission WITH I2C STOP message
}
