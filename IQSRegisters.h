#ifndef IQS_REGISTERS_H
#define IQS_REGISTERS_H

#include <string>
#include <unordered_map>
#include <Arduino.h>

class IQSRegister
{
    private:
        int _address;
        int _numBytes;
        std::string _name;
        std::string _description;
        char _mode; // 'r' = read, 'w' = write, 'b' = read/write

        // dataType
        // 0 = byte (raw 8 bit value)
        // 1 = unsigned int (8 or 16 bit value) [DEFAULT]
        // 2 = signed int (16 bit value, one sign bit, 15 data bits)
        int _dataType;

    public:
        IQSRegister(int address, int numBytes, std::string name, std::string description, char mode = 'r', int dataType = 1);
        IQSRegister(int address, int numBytes, char mode = 'r', int dataType = 1);
        IQSRegister();
        const int getAddress() { return _address; }
        const int getNumBytes() { return _numBytes; }
        const std::string getName() { return _name; }
        const std::string getDescription() { return _description; }
        const char getMode() { return _mode; }
        const int getDataType() { return _dataType; }
        void getAddressAsByteArray(byte *byteArray) const;

        byte read(int device_address, byte* buf);
        byte read(int device_address, byte* buf, int numBytesToRead);
        int read(int device_address);
        int read(int device_address, byte &error);

        byte write(int device_address, byte* data);
        byte write(int device_address, int value);
};

static std::unordered_map<int, IQSRegister> IMPORTANT_IQS_REGISTERS
({
    // board info
    {0x0000, IQSRegister(0x0000, 2, "Product Number", "Product Number")},
    {0x0002, IQSRegister(0x0002, 2, "Project Number", "Project Number")},
    {0x0004, IQSRegister(0x0004, 1, "Major Version", "Major Version")},
    {0x0005, IQSRegister(0x0005, 1, "Minor Version", "Minor Version")},
    {0x0006, IQSRegister(0x0006, 1, "Bootloader Status", "Bootloader Status", 'r', 0)},

    // misc system status info
    {0x000B, IQSRegister(0x000B, 1, "Max Touch", "First four bits: max touch column. Last four bits: max touch row.", 'r', 0)},
    {0x000C, IQSRegister(0x000C, 1, "Previous Cycle Time", "Previous Cycle Time (ms)")},
    {0x0010, IQSRegister(0x0010, 1, "System Info 1", "bit 7: unused, bit 6: unused, bit 5: SWITCH_STATE, bit 4: SNAP_TOGGLE, bit 3: RR_MISSED, bit 2: TOO_MANY_FINGERS, bit 1: PALM_DETECT, bit 0: TP_MOVEMENT", 'r', 0)},

    // finger touch data
    {0x0011, IQSRegister(0x0011, 1, "Number of Finger Touches", "Number of Finger Touches")},

    // finger 1 (only finger 1 has relative x/y data)
    {0x0012, IQSRegister(0x0012, 2, "Finger 1 Relative X", "Finger 1 Relative X (pixels)", 'r', 2)},
    {0x0014, IQSRegister(0x0014, 2, "Finger 1 Relative Y", "Finger 1 Relative Y (pixels)", 'r', 2)},
    {0x0016, IQSRegister(0x0016, 2, "Finger 1 Absolute X", "Finger 1 Absolute X (pixels)")},
    {0x0018, IQSRegister(0x0018, 2, "Finger 1 Absolute Y", "Finger 1 Absolute Y (pixels)")},
    {0x001A, IQSRegister(0x001A, 2, "Finger 1 Touch Strength", "Finger 1 Touch Strength")},
    {0x001C, IQSRegister(0x001C, 1, "Finger 1 Touch Area", "Finger 1 Touch Area")},
    // finger 2
    {0x001D, IQSRegister(0x001D, 2, "Finger 2 Absolute X", "Finger 2 Absolute X (pixels)")},
    {0x001F, IQSRegister(0x001F, 2, "Finger 2 Absolute Y", "Finger 2 Absolute Y (pixels)")},
    {0x0021, IQSRegister(0x0021, 2, "Finger 2 Touch Strength", "Finger 2 Touch Strength")},
    {0x0023, IQSRegister(0x0023, 1, "Finger 2 Touch Area", "Finger 2 Touch Area")},
    // finger 3
    {0x0024, IQSRegister(0x0024, 2, "Finger 3 Absolute X", "Finger 3 Absolute X (pixels)")},
    {0x0026, IQSRegister(0x0026, 2, "Finger 3 Absolute Y", "Finger 3 Absolute Y (pixels)")},
    {0x0028, IQSRegister(0x0028, 2, "Finger 3 Touch Strength", "Finger 3 Touch Strength")},
    {0x002A, IQSRegister(0x002A, 1, "Finger 3 Touch Area", "Finger 3 Touch Area")},
    // finger 4
    {0x002B, IQSRegister(0x002B, 2, "Finger 4 Absolute X", "Finger 4 Absolute X (pixels)")},
    {0x002D, IQSRegister(0x002D, 2, "Finger 4 Absolute Y", "Finger 4 Absolute Y (pixels)")},
    {0x002F, IQSRegister(0x002F, 2, "Finger 4 Touch Strength", "Finger 4 Touch Strength")},
    {0x0031, IQSRegister(0x0031, 1, "Finger 4 Touch Area", "Finger 4 Touch Area")},
    // finger 5
    {0x0032, IQSRegister(0x0032, 2, "Finger 5 Absolute X", "Finger 5 Absolute X (pixels)")},
    {0x0034, IQSRegister(0x0034, 2, "Finger 5 Absolute Y", "Finger 5 Absolute Y (pixels)")},
    {0x0036, IQSRegister(0x0036, 2, "Finger 5 Touch Strength", "Finger 5 Touch Strength")},
    {0x0038, IQSRegister(0x0038, 1, "Finger 5 Touch Area", "Finger 5 Touch Area")},

    // settings
    {0x066E, IQSRegister(0x066E, 2, "X Resolution", "X Resolution", 'b')},
    {0x0670, IQSRegister(0x0670, 2, "Y Resolution", "Y Resolution", 'b')},
    {0x057A, IQSRegister(0x057A, 2, "Active Mode Report Rate", "Active Mode Report Rate (ms)", 'b')},
    {0x057C, IQSRegister(0x057C, 2, "Idle Touch Mode Report Rate", "Idle Touch Mode Report Rate (ms)", 'b')},
    {0x057E, IQSRegister(0x057E, 2, "Idle Mode Report Rate", "Idle Mode Report Rate (ms)", 'b')},
    {0x0580, IQSRegister(0x0580, 2, "LP1 Mode Report Rate", "Low Power 1 Mode Report Rate (ms)", 'b')},
    {0x0582, IQSRegister(0x0582, 2, "LP2 Mode Report Rate", "Low Power 2 Mode Report Rate (ms)", 'b')},
    {0x058A, IQSRegister(0x058A, 1, "I2C Timeout", "I2C Timeout (ms)", 'b')},
    {0x0669, IQSRegister(0x0669, 1, "XY Config 0", "bits 7-4: unused, bit 3: PALM_REJECT, bit 2: SWITCH_XY_AXIS, bit 1: FLIP_Y, bit 0: FLIP_X", 'b', 0)},
    {0x066A, IQSRegister(0x066A, 1, "Max multi-touches", "Maximum number of concurrent fingers", 'b', 1)},
    {0x0675, IQSRegister(0x0675, 2, "Default read address", "Default read address", 'b')},

    // gesture data
    {0x000D, IQSRegister(0x000D, 1, "Single Finger Gestures", "bit 7: unused, bit 6: unused, bit 5: SWIPE Y-, bit 4: SWIPE Y+, bit 3: SWIPE X+, bit 2: SWIPE X-, bit 1: PRESS AND HOLD, bit 0: SINGLE TAP", 'r', 0)},
    {0x000E, IQSRegister(0x000E, 1, "Multi Finger Gestures", "bit 7: unused, bit 6: unused, bit 5: unused, bit 4: unused, bit 3: unused, bit 2: ZOOM, bit 1: SCROLL, bit 0: TWO FINGER TAP", 'r', 0)},

    // gesture settings
    {0x06B7, IQSRegister(0x06B7, 1, "Single Finger Gestures Settings", "bit 7: unused, bit 6: unused, bit 5: SWIPE Y-, bit 4: SWIPE Y+, bit 3: SWIPE X+, bit 2: SWIPE X-, bit 1: PRESS AND HOLD, bit 0: SINGLE TAP | 0 disables, 1 enables", 'b', 0)},
    {0x06B8, IQSRegister(0x06B8, 1, "Multi Finger Gestures Settings", "bit 7: unused, bit 6: unused, bit 5: unused, bit 4: unused, bit 3: unused, bit 2: ZOOM, bit 1: SCROLL, bit 0: TWO FINGER TAP | 0 disables, 1 enables", 'b', 0)},
});

class IQSRegisters
{
    private:
        // use enum trick to initialize array in class definition
    public:
        static IQSRegister getRegister(int address);
        static IQSRegister getRegister(std::string name);

        // often used registers

        // touch data
        static IQSRegister NumFingers;
        static IQSRegister Finger1RelativeX;
        static IQSRegister Finger1RelativeY;
        static IQSRegister Finger1AbsoluteX;
        static IQSRegister Finger1AbsoluteY;
        static IQSRegister Finger1TouchStrength;
        static IQSRegister Finger1TouchArea;
        static IQSRegister Finger2AbsoluteX;
        static IQSRegister Finger2AbsoluteY;
        static IQSRegister Finger2TouchStrength;
        static IQSRegister Finger2TouchArea;
        static IQSRegister Finger3AbsoluteX;
        static IQSRegister Finger3AbsoluteY;
        static IQSRegister Finger3TouchStrength;
        static IQSRegister Finger3TouchArea;
        static IQSRegister Finger4AbsoluteX;
        static IQSRegister Finger4AbsoluteY;
        static IQSRegister Finger4TouchStrength;
        static IQSRegister Finger4TouchArea;
        static IQSRegister Finger5AbsoluteX;
        static IQSRegister Finger5AbsoluteY;
        static IQSRegister Finger5TouchStrength;
        static IQSRegister Finger5TouchArea;

        // settings
        static IQSRegister XResolution;
        static IQSRegister YResolution;
        static IQSRegister ActiveModeReportRate;
        static IQSRegister IdleTouchModeReportRate;
        static IQSRegister IdleModeReportRate;
        static IQSRegister LP1ModeReportRate;
        static IQSRegister LP2ModeReportRate;
        static IQSRegister I2CTimeout;
        static IQSRegister XYConfig0;
        static IQSRegister MaxMultiTouches;
        static IQSRegister DefaultReadAddress;

        // flags
        static IQSRegister SingleFingerGestures;
        static IQSRegister MultiFingerGestures;
        static IQSRegister SystemInfo1;

        // system status
        static IQSRegister PreviousCycleTime;

};

#endif // IQS_REGISTERS_H
