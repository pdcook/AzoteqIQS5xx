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
        char _mode; // 'r' = read, 'w' = write, 'b' = read/write

    public:
        IQSRegister(int address, int numBytes, std::string name, std::string description, char mode = 'r');
        IQSRegister();
        int getAddress() const;
        int getNumBytes() const;
        void getAddressAsByteArray(byte *byteArray) const;
        std::string getName() const;
        std::string getDescription() const;
        char getMode() const;

        byte read(int device_address, byte* buf);
        int read(int device_address);

        byte write(int device_address, byte* data);
        byte write(int device_address, int value);
};

const static IQSRegister IMPORTANT_IQS_REGISTERS[] =
{
    // board info
    IQSRegister(0x0000, 2, "Product Number", "Product Number"),
    IQSRegister(0x0002, 2, "Project Number", "Project Number"),
    IQSRegister(0x0004, 1, "Major Version", "Major Version"),
    IQSRegister(0x0005, 1, "Minor Version", "Minor Version"),
    IQSRegister(0x0006, 1, "Bootloader Status", "Bootloader Status"),

    // misc system status info
    IQSRegister(0x000B, 1, "Max Touch", "First four bits: max touch column. Last four bits: max touch row."),
    IQSRegister(0x000C, 1, "Previous Cycle Time", "Previous Cycle Time (ms)"),
    IQSRegister(0x0010, 1, "System Info 1", "bit 7: unused, bit 6: unused, bit 5: SWITCH_STATE, bit 4: SNAP_TOGGLE, bit 3: RR_MISSED, bit 2: TOO_MANY_FINGERS, bit 1: PALM_DETECT, bit 0: TP_MOVEMENT"),

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

    // settings
    IQSRegister(0x066E, 2, "X Resolution", "X Resolution", 'b'),
    IQSRegister(0x0670, 2, "Y Resolution", "Y Resolution", 'b'),
    IQSRegister(0x057A, 2, "Active Mode Report Rate", "Active Mode Report Rate (ms)", 'b'),
    IQSRegister(0x057C, 2, "Idle Touch Mode Report Rate", "Idle Touch Mode Report Rate (ms)", 'b'),
    IQSRegister(0x057E, 2, "Idle Mode Report Rate", "Idle Mode Report Rate (ms)", 'b'),
    IQSRegister(0x0580, 2, "LP1 Mode Report Rate", "Low Power 1 Mode Report Rate (ms)", 'b'),
    IQSRegister(0x0582, 2, "LP2 Mode Report Rate", "Low Power 2 Mode Report Rate (ms)", 'b'),
    IQSRegister(0x058A, 1, "I2C Timeout", "I2C Timeout (ms)", 'b'),

    // gesture data (data is replicated in two registers)
    IQSRegister(0x000D, 1, "Single Finger Gestures", "bit 7: unused, bit 6: unused, bit 5: SWIPE Y-, bit 4: SWIPE Y+, bit 3: SWIPE X+, bit 2: SWIPE X-, bit 1: TAP AND HOLD, bit 0: SINGLE TAP"),
    IQSRegister(0x000E, 1, "Multi Finger Gestures", "bit 7: unused, bit 6: unused, bit 5: unused, bit 4: unused, bit 3: unused, bit 2: ZOOM, bit 1: SCROLL, bit 0: TWO FINGER TAP"),
    IQSRegister(0x06B7, 1, "Single Finger Gestures", "bit 7: unused, bit 6: unused, bit 5: SWIPE Y-, bit 4: SWIPE Y+, bit 3: SWIPE X+, bit 2: SWIPE X-, bit 1: TAP AND HOLD, bit 0: SINGLE TAP"),
    IQSRegister(0x06B8, 1, "Multi Finger Gestures", "bit 7: unused, bit 6: unused, bit 5: unused, bit 4: unused, bit 3: unused, bit 2: ZOOM, bit 1: SCROLL, bit 0: TWO FINGER TAP"),

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
