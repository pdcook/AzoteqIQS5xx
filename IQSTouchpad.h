#ifndef IQS_TOUCHPAD_H
#define IQS_TOUCHPAD_H

#include <Arduino.h>
#include "IQSRegisters.h"
#include <vector>

#define DEFAULT_I2C_ADDRESS 0x74


class IQSTouchpad
{
    private:
        byte _i2cAddress;
        int _PIN_RDY;
        int _PIN_RST;

        // registers
        IQSRegister _numFingersReg;
        IQSRegister _finger1XReg;
        IQSRegister _finger1YReg;
        IQSRegister _finger1StrengthReg;
        IQSRegister _finger1AreaReg;
        IQSRegister _finger2XReg;
        IQSRegister _finger2YReg;
        IQSRegister _finger2StrengthReg;
        IQSRegister _finger2AreaReg;
        IQSRegister _finger3XReg;
        IQSRegister _finger3YReg;
        IQSRegister _finger3StrengthReg;
        IQSRegister _finger3AreaReg;
        IQSRegister _finger4XReg;
        IQSRegister _finger4YReg;
        IQSRegister _finger4StrengthReg;
        IQSRegister _finger4AreaReg;
        IQSRegister _finger5XReg;
        IQSRegister _finger5YReg;
        IQSRegister _finger5StrengthReg;
        IQSRegister _finger5AreaReg;


    public:
        IQSTouchpad(int PIN_RDY, int PIN_RST, byte i2cAddress = DEFAULT_I2C_ADDRESS);
        void begin();
        void reset();
        void endCommunicationWindow();
        void update();
        byte getI2CAddress();
        int getPIN_RDY();
        int getPIN_RST();
        bool isReady();

        int numFingers;
        int finger1X;
        int finger1Y;
        int finger1Strength;
        int finger1Area;
        int finger2X;
        int finger2Y;
        int finger2Strength;
        int finger2Area;
        int finger3X;
        int finger3Y;
        int finger3Strength;
        int finger3Area;
        int finger4X;
        int finger4Y;
        int finger4Strength;
        int finger4Area;
        int finger5X;
        int finger5Y;
        int finger5Strength;
        int finger5Area;

        static std::vector<IQSTouchpad*> _touchpads;
        bool _ready = false;

};

#endif // IQS_TOUCHPAD_H
