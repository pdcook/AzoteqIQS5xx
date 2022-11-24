#ifndef IQS_REGISTERS_H
#define IQS_REGISTERS_H

#include <string>
#include <Arduino.h>

class IQSRegister
{
    private:
        int _address;
        int _numBytes;
        std::string _name;
        std::string _description;

    public:
        IQSRegister(int address, int numBytes, std::string name, std::string description);
        IQSRegister();
        int getAddress() const;
        int getNumBytes() const;
        void getAddressAsByteArray(byte *byteArray) const;
        std::string getName() const;
        std::string getDescription() const;

        byte read(int device_address, byte* buf);
};

const static IQSRegister IMPORTANT_IQS_REGISTERS[] =
{
    // board info
    IQSRegister(0x0000, 2, "Product Number", "Product Number"),
    IQSRegister(0x0002, 2, "Project Number", "Project Number"),
    IQSRegister(0x0004, 1, "Major Version", "Major Version"),
    IQSRegister(0x0005, 1, "Minor Version", "Minor Version"),
    IQSRegister(0x0006, 1, "Bootloader Status", "Bootloader Status"),

    // misc board status info
    IQSRegister(0x000B, 1, "Max Touch", "First four bits: max touch column. Last four bits: max touch row."),
    IQSRegister(0x000C, 1, "Previous Cycle Time", "Previous Cycle Time (ms)"),

    // finger touch data
    IQSRegister(0x0011, 1, "Number of Finger Touches", "Number of Finger Touches"),

    // finger 1 (only finger 1 has relative x/y data)
    IQSRegister(0x0012, 2, "Finger 1 Relative X", "Finger 1 Relative X (pixels)"),
    IQSRegister(0x0014, 2, "Finger 1 Relative Y", "Finger 1 Relative Y (pixels)"),
    IQSRegister(0x0016, 2, "Finger 1 Absolute X", "Finger 1 Absolute X (pixels)"),
    IQSRegister(0x0018, 2, "Finger 1 Absolute Y", "Finger 1 Absolute Y (pixels)"),
    IQSRegister(0x001A, 2, "Finger 1 Touch Strength", "Finger 1 Touch Strength"),
    IQSRegister(0x001C, 1, "Finger 1 Touch Area", "Finger 1 Touch Area"),
    // finger 2
    IQSRegister(0x001D, 2, "Finger 2 Absolute X", "Finger 2 Absolute X (pixels)"),
    IQSRegister(0x001F, 2, "Finger 2 Absolute Y", "Finger 2 Absolute Y (pixels)"),
    IQSRegister(0x0021, 2, "Finger 2 Touch Strength", "Finger 2 Touch Strength"),
    IQSRegister(0x0023, 1, "Finger 2 Touch Area", "Finger 2 Touch Area"),
    // finger 3
    IQSRegister(0x0024, 2, "Finger 3 Absolute X", "Finger 3 Absolute X (pixels)"),
    IQSRegister(0x0026, 2, "Finger 3 Absolute Y", "Finger 3 Absolute Y (pixels)"),
    IQSRegister(0x0028, 2, "Finger 3 Touch Strength", "Finger 3 Touch Strength"),
    IQSRegister(0x002A, 1, "Finger 3 Touch Area", "Finger 3 Touch Area"),
    // finger 4
    IQSRegister(0x002B, 2, "Finger 4 Absolute X", "Finger 4 Absolute X (pixels)"),
    IQSRegister(0x002D, 2, "Finger 4 Absolute Y", "Finger 4 Absolute Y (pixels)"),
    IQSRegister(0x002F, 2, "Finger 4 Touch Strength", "Finger 4 Touch Strength"),
    IQSRegister(0x0031, 1, "Finger 4 Touch Area", "Finger 4 Touch Area"),
    // finger 5
    IQSRegister(0x0032, 2, "Finger 5 Absolute X", "Finger 5 Absolute X (pixels)"),
    IQSRegister(0x0034, 2, "Finger 5 Absolute Y", "Finger 5 Absolute Y (pixels)"),
    IQSRegister(0x0036, 2, "Finger 5 Touch Strength", "Finger 5 Touch Strength"),
    IQSRegister(0x0038, 1, "Finger 5 Touch Area", "Finger 5 Touch Area"),

};

class IQSRegisters
{
    private:
        // use enum trick to initialize array in class definition
    public:
        static IQSRegister getRegister(int address);
        static IQSRegister getRegister(std::string name);
};

#endif // IQS_REGISTERS_H
