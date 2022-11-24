#include "IQSRegisters.h"
#include "I2CHelpers.h"

IQSRegister::IQSRegister()
{
    //ctor
}

IQSRegister::IQSRegister(int address, int numBytes, std::string name, std::string description)
{
    this->_address = address;
    this->_numBytes = numBytes;
    this->_name = name;
    this->_description = description;
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

byte IQSRegister::read(int device_address, byte* buf)
{
    return I2CHelpers::readFromRegister(device_address, this->_address, this->_numBytes, buf);
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
