#include "IQSTouchpad.h"
#include <Arduino.h>
#include <Wire.h>
#include "I2CHelpers.h"
#include "IQSRegisters.h"
#include <vector>

std::vector<IQSTouchpad*> IQSTouchpad::_touchpads = std::vector<IQSTouchpad*>();

IQSTouchpad::IQSTouchpad(int PIN_RDY, int PIN_RST, byte i2cAddress)
{
    this->_PIN_RDY = PIN_RDY;
    this->_PIN_RST = PIN_RST;
    this->_i2cAddress = i2cAddress;

    // store registers of interest
    this->_numFingersReg = IQSRegisters::getRegister(0x0011);

    // finger 1
    this->_finger1XReg = IQSRegisters::getRegister(0x0016);
    this->_finger1YReg = IQSRegisters::getRegister(0x0018);
    this->_finger1StrengthReg = IQSRegisters::getRegister(0x001A);
    this->_finger1AreaReg = IQSRegisters::getRegister(0x001C);

    // finger 2
    this->_finger2XReg = IQSRegisters::getRegister(0x001D);
    this->_finger2YReg = IQSRegisters::getRegister(0x001F);
    this->_finger2StrengthReg = IQSRegisters::getRegister(0x0021);
    this->_finger2AreaReg = IQSRegisters::getRegister(0x0023);

    // finger 3
    this->_finger3XReg = IQSRegisters::getRegister(0x0024);
    this->_finger3YReg = IQSRegisters::getRegister(0x0026);
    this->_finger3StrengthReg = IQSRegisters::getRegister(0x0028);
    this->_finger3AreaReg = IQSRegisters::getRegister(0x002A);

    // finger 4
    this->_finger4XReg = IQSRegisters::getRegister(0x002B);
    this->_finger4YReg = IQSRegisters::getRegister(0x002D);
    this->_finger4StrengthReg = IQSRegisters::getRegister(0x002F);
    this->_finger4AreaReg = IQSRegisters::getRegister(0x0031);

    // finger 5
    this->_finger5XReg = IQSRegisters::getRegister(0x0032);
    this->_finger5YReg = IQSRegisters::getRegister(0x0034);
    this->_finger5StrengthReg = IQSRegisters::getRegister(0x0036);
    this->_finger5AreaReg = IQSRegisters::getRegister(0x0038);

    // add this touchpad to the list of touchpads
    IQSTouchpad::_touchpads.push_back(this);
}

void IRAM_ATTR IQSInterruptHandler()
{
    // loop through all the touchpads and find the one that triggered the interrupt
    for (int i = 0; i < IQSTouchpad::_touchpads.size(); i++)
    {
        IQSTouchpad *touchpad = IQSTouchpad::_touchpads[i];
        if (digitalRead(touchpad->getPIN_RDY()))
        {
            if (!touchpad->_ready)
            {
                touchpad->_ready = true;
            }
        }
    }
}

void IQSTouchpad::reset()
{
    // Reset the touchpad
    digitalWrite(this->_PIN_RST, LOW);
    delay(200);
    digitalWrite(this->_PIN_RST, HIGH);
    delay(200);
}

void IQSTouchpad::begin()
{
    Wire.begin();

    pinMode(this->_PIN_RDY, INPUT);
    pinMode(this->_PIN_RST, OUTPUT);

    // reset the touchpad
    this->reset();

    // attach interrupt to RDY pin
    attachInterrupt(this->_PIN_RDY, IQSInterruptHandler, RISING);
}

void IQSTouchpad::endCommunicationWindow()
{
    // end communication window
    I2CHelpers::endCommunication(this->_i2cAddress);
}

byte IQSTouchpad::getI2CAddress()
{
    return this->_i2cAddress;
}

int IQSTouchpad::getPIN_RDY()
{
    return this->_PIN_RDY;
}

int IQSTouchpad::getPIN_RST()
{
    return this->_PIN_RST;
}

bool IQSTouchpad::isReady()
{
    return this->_ready;
}

void IQSTouchpad::update()
{
    if (this->_ready)
    {


        /*
         * Read the following registers:
         * - Number of fingers
         *
         * - Then the data for each finger that is present:
         *   - Absolute X
         *   - Absolute Y
         *   - Touch Strength
         *   - Touch Area
         */

        // for now, just read the number of fingers
        byte numFingers[1];
        this->_numFingersReg.read(this->_i2cAddress, numFingers);
        this->numFingers = numFingers[0];

        // end communication window
        this->endCommunicationWindow();

    }

    // reset ready flag
    this->_ready = false;
}
