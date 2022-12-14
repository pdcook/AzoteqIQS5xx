#include "IQSRegisters.h"
#include "I2CHelpers.h"
#include <stdexcept>
#include <Arduino.h>

IQSRegister::IQSRegister()
{
    //ctor
}

IQSRegister::IQSRegister(int address, int numBytes, char mode, int dataType)
{
    this->_address = address;
    this->_numBytes = numBytes;
    this->_mode = mode;
    this->_name = "Unknown";
    this->_description = "Unknown";
    this->_dataType = dataType;
}

IQSRegister::IQSRegister(int address, int numBytes, std::string name, std::string description, char mode, int dataType)
{
    this->_address = address;
    this->_numBytes = numBytes;
    this->_name = name;
    this->_description = description;
    this->_mode = mode;
    this->_dataType = dataType;
}

void IQSRegister::getAddressAsByteArray(byte *byteArray)
{
    I2CHelpers::intToTwoByteArray(this->_address, byteArray);
}

byte IQSRegister::read(int device_address, byte* buf)
{
    return I2CHelpers::readFromRegister(device_address, this->_address, this->_numBytes, buf);
}

byte IQSRegister::read(int device_address, byte* buf, int numBytesToRead)
{
    return I2CHelpers::readFromRegister(device_address, this->_address, numBytesToRead, buf);
}

int IQSRegister::read(int device_address)
{
    byte error;
    return this->read(device_address, error);
}

int IQSRegister::read(int device_address, byte &error)
{
    if (this->_mode == 'w')
    {
        // cannot read from a write-only register
        // throw exception
        //throw std::invalid_argument("Cannot read from a write-only register");
        error = 8;
        return 0;
    }
    byte buf[this->_numBytes];
    error = this->read(device_address, buf);
    if (this->_dataType == 0 || this->_numBytes == 1)
    {
        int value = buf[0];
        return value;
    }
    else if (this->_dataType == 1)
    {
        int value = I2CHelpers::byteArrayToInt(buf, this->_numBytes);
        return value;
    }
    else if (this->_dataType == 2)
    {
        int value = I2CHelpers::byteArrayToSignedInt(buf, this->_numBytes);
        return value;
    }
    else
    {
        // throw exception
        //throw std::invalid_argument("Invalid data type or number of bytes");
        error = 9;
        return 0;
    }
}

byte IQSRegister::write(int device_address, byte* buf)
{
    if (this->_mode == 'r')
    {
        // cannot write to a read-only register
        // throw exception
        //throw std::invalid_argument("Cannot write to a read-only register");
        return 8;
    }
    return I2CHelpers::writeToRegister(device_address, this->_address, this->_numBytes, buf);
}

byte IQSRegister::write(int device_address, int value)
{
    if (this->_mode == 'r')
    {
        // cannot write to a read-only register
        // throw exception
        //throw std::invalid_argument("Cannot write to a read-only register");
        return 8;
    }
    if (this->_numBytes == 1)
    {
        byte buf[1];
        buf[0] = value;
        byte error = I2CHelpers::writeToRegister(device_address, this->_address, this->_numBytes, buf);
        return error;
    }
    else if (this->_numBytes == 2)
    {
        byte buf[2];
        I2CHelpers::intToTwoByteArray(value, buf);
        byte error = I2CHelpers::writeToRegister(device_address, this->_address, this->_numBytes, buf);
        return error;
    }
    else
    {
        // unimplemented, throw exception
        //throw std::invalid_argument("Unimplemented");
        return 9;
    }
}

IQSRegister* IQSRegisters::getRegister(int address)
{
    return IMPORTANT_IQS_REGISTERS[address];
}

IQSRegister* IQSRegisters::getRegister(std::string name)
{
    for (auto pair : IMPORTANT_IQS_REGISTERS)
    {
        if (pair.second->getName() == name)
        {
            return pair.second;
        }
    }
    return new IQSRegister(-1, -1, "Unknown", "Unknown");
}

IQSRegister* IQSRegisters::NumFingers           = IMPORTANT_IQS_REGISTERS[0x0011];
IQSRegister* IQSRegisters::Finger1RelativeX     = IMPORTANT_IQS_REGISTERS[0x0012];
IQSRegister* IQSRegisters::Finger1RelativeY     = IMPORTANT_IQS_REGISTERS[0x0014];
IQSRegister* IQSRegisters::Finger1AbsoluteX     = IMPORTANT_IQS_REGISTERS[0x0016];
IQSRegister* IQSRegisters::Finger1AbsoluteY     = IMPORTANT_IQS_REGISTERS[0x0018];
IQSRegister* IQSRegisters::Finger1TouchStrength = IMPORTANT_IQS_REGISTERS[0x001A];
IQSRegister* IQSRegisters::Finger1TouchArea     = IMPORTANT_IQS_REGISTERS[0x001C];

IQSRegister* IQSRegisters::Finger2AbsoluteX     = IMPORTANT_IQS_REGISTERS[0x001D];
IQSRegister* IQSRegisters::Finger2AbsoluteY     = IMPORTANT_IQS_REGISTERS[0x001F];
IQSRegister* IQSRegisters::Finger2TouchStrength = IMPORTANT_IQS_REGISTERS[0x0021];
IQSRegister* IQSRegisters::Finger2TouchArea     = IMPORTANT_IQS_REGISTERS[0x0023];

IQSRegister* IQSRegisters::Finger3AbsoluteX     = IMPORTANT_IQS_REGISTERS[0x0024];
IQSRegister* IQSRegisters::Finger3AbsoluteY     = IMPORTANT_IQS_REGISTERS[0x0026];
IQSRegister* IQSRegisters::Finger3TouchStrength = IMPORTANT_IQS_REGISTERS[0x0028];
IQSRegister* IQSRegisters::Finger3TouchArea     = IMPORTANT_IQS_REGISTERS[0x002A];

IQSRegister* IQSRegisters::Finger4AbsoluteX     = IMPORTANT_IQS_REGISTERS[0x002B];
IQSRegister* IQSRegisters::Finger4AbsoluteY     = IMPORTANT_IQS_REGISTERS[0x002D];
IQSRegister* IQSRegisters::Finger4TouchStrength = IMPORTANT_IQS_REGISTERS[0x002F];
IQSRegister* IQSRegisters::Finger4TouchArea     = IMPORTANT_IQS_REGISTERS[0x0031];

IQSRegister* IQSRegisters::Finger5AbsoluteX     = IMPORTANT_IQS_REGISTERS[0x0032];
IQSRegister* IQSRegisters::Finger5AbsoluteY     = IMPORTANT_IQS_REGISTERS[0x0034];
IQSRegister* IQSRegisters::Finger5TouchStrength = IMPORTANT_IQS_REGISTERS[0x0036];
IQSRegister* IQSRegisters::Finger5TouchArea     = IMPORTANT_IQS_REGISTERS[0x0038];

// settings
IQSRegister* IQSRegisters::XResolution             = IMPORTANT_IQS_REGISTERS[0x066E];
IQSRegister* IQSRegisters::YResolution             = IMPORTANT_IQS_REGISTERS[0x0670];
IQSRegister* IQSRegisters::ActiveModeReportRate    = IMPORTANT_IQS_REGISTERS[0x057A];
IQSRegister* IQSRegisters::IdleTouchModeReportRate = IMPORTANT_IQS_REGISTERS[0x057C];
IQSRegister* IQSRegisters::IdleModeReportRate      = IMPORTANT_IQS_REGISTERS[0x057E];
IQSRegister* IQSRegisters::LP1ModeReportRate       = IMPORTANT_IQS_REGISTERS[0x0580];
IQSRegister* IQSRegisters::LP2ModeReportRate       = IMPORTANT_IQS_REGISTERS[0x0582];
IQSRegister* IQSRegisters::I2CTimeout              = IMPORTANT_IQS_REGISTERS[0x058A];
IQSRegister* IQSRegisters::XYConfig0               = IMPORTANT_IQS_REGISTERS[0x0669];
IQSRegister* IQSRegisters::MaxMultiTouches         = IMPORTANT_IQS_REGISTERS[0x066A];
IQSRegister* IQSRegisters::DefaultReadAddress      = IMPORTANT_IQS_REGISTERS[0x0675];

// flags
IQSRegister* IQSRegisters::SingleFingerGestures = IMPORTANT_IQS_REGISTERS[0x000D];
IQSRegister* IQSRegisters::MultiFingerGestures  = IMPORTANT_IQS_REGISTERS[0x000E];
IQSRegister* IQSRegisters::SystemInfo1          = IMPORTANT_IQS_REGISTERS[0x0010];

// system status
IQSRegister* IQSRegisters::PreviousCycleTime = IMPORTANT_IQS_REGISTERS[0x000C];
