#include "IQSRegisters.h"
#include "I2CHelpers.h"
#include <stdexcept>

IQSRegister::IQSRegister()
{
    //ctor
}

IQSRegister::IQSRegister(int address, int numBytes, std::string name, std::string description, char mode)
{
    this->_address = address;
    this->_numBytes = numBytes;
    this->_name = name;
    this->_description = description;
    this->_mode = mode;
}

int IQSRegister::getAddress() const
{
    return this->_address;
}

int IQSRegister::getNumBytes() const
{
    return this->_numBytes;
}

void IQSRegister::getAddressAsByteArray(byte *byteArray) const
{
    I2CHelpers::intToTwoByteArray(this->_address, byteArray);
}

std::string IQSRegister::getName() const
{
    return this->_name;
}

std::string IQSRegister::getDescription() const
{
    return this->_description;
}

char IQSRegister::getMode() const
{
    return this->_mode;
}

byte IQSRegister::read(int device_address, byte* buf)
{
    return I2CHelpers::readFromRegister(device_address, this->_address, this->_numBytes, buf);
}

int IQSRegister::read(int device_address)
{
    if (this->_mode == 'w')
    {
        // cannot read from a write-only register
        // throw exception
        throw std::invalid_argument("Cannot read from a write-only register");
    }
    byte buf[this->_numBytes];
    this->read(device_address, buf);
    return I2CHelpers::byteArrayToInt(buf, this->_numBytes);
}

byte IQSRegister::write(int device_address, byte* buf)
{
    if (this->_mode == 'r')
    {
        // cannot write to a read-only register
        // throw exception
        throw std::invalid_argument("Cannot write to a read-only register");
    }
    return I2CHelpers::writeToRegister(device_address, this->_address, this->_numBytes, buf);
}

byte IQSRegister::write(int device_address, int value)
{
    if (this->_mode == 'r')
    {
        // cannot write to a read-only register
        // throw exception
        throw std::invalid_argument("Cannot write to a read-only register");
    }
    if (this->_numBytes == 1)
    {
        byte buf[1];
        buf[0] = value;
        return I2CHelpers::writeToRegister(device_address, this->_address, this->_numBytes, buf);
    }
    else if (this->_numBytes == 2)
    {
        byte buf[2];
        I2CHelpers::intToTwoByteArray(value, buf);
        return I2CHelpers::writeToRegister(device_address, this->_address, this->_numBytes, buf);
    }
    else
    {
        // unimplemented, throw exception
        throw std::invalid_argument("Unimplemented");
    }
}

IQSRegister IQSRegisters::getRegister(int address)
{
    for (int i = 0; i < sizeof(IMPORTANT_IQS_REGISTERS) / sizeof(IQSRegister); i++)
    {
        if (IMPORTANT_IQS_REGISTERS[i].getAddress() == address)
        {
            return IMPORTANT_IQS_REGISTERS[i];
        }
    }
    return IQSRegister(0, 0, "Unknown", "Unknown");
}

IQSRegister IQSRegisters::getRegister(std::string name)
{
    for (int i = 0; i < sizeof(IMPORTANT_IQS_REGISTERS) / sizeof(IQSRegister); i++)
    {
        if (IMPORTANT_IQS_REGISTERS[i].getName() == name)
        {
            return IMPORTANT_IQS_REGISTERS[i];
        }
    }
    return IQSRegister(0, 0, "Unknown", "Unknown");
}
