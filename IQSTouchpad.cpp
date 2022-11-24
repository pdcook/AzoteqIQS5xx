#include "IQSTouchpad.h"
#include <Arduino.h>
#include <Wire.h>
#include "I2CHelpers.h"
#include "IQSRegisters.h"
#include <vector>
#include "Finger.h"
#include <stdexcept>

std::vector<IQSTouchpad*> IQSTouchpad::_touchpads = std::vector<IQSTouchpad*>();

IQSTouchpad::IQSTouchpad(int PIN_RDY, int PIN_RST, int X_resolution, int Y_resolution, byte i2cAddress)
{
    this->_initialized = false;

    this->_PIN_RDY = PIN_RDY;
    this->_PIN_RST = PIN_RST;
    this->_i2cAddress = i2cAddress;
    this->_X_res_to_set = X_resolution;
    this->_Y_res_to_set = Y_resolution;

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

    // misc system info
    this->_PreviousCycleTimeReg = IQSRegisters::getRegister(0x000C);

    // flag registers
    this->_SystemInfo1Reg = IQSRegisters::getRegister(0x0010);
    this->_SingleFingerGestureReg = IQSRegisters::getRegister(0x000D);
    this->_MultiFingerGestureReg = IQSRegisters::getRegister(0x000E);

    // settings registers
    this->_XResolutionReg = IQSRegisters::getRegister(0x066E);
    this->_YResolutionReg = IQSRegisters::getRegister(0x0670);
    this->_ReportRateActiveModeReg = IQSRegisters::getRegister(0x057A);
    this->_ReportRateIdleTouchModeReg = IQSRegisters::getRegister(0x057C);
    this->_ReportRateIdleModeReg = IQSRegisters::getRegister(0x057E);
    this->_ReportRateLP1ModeReg = IQSRegisters::getRegister(0x0580);
    this->_ReportRateLP2ModeReg = IQSRegisters::getRegister(0x0582);
    this->_I2CTimeoutReg = IQSRegisters::getRegister(0x058A);

    // add this touchpad to the list of touchpads
    IQSTouchpad::_touchpads.push_back(this);
}

void IQSTouchpad::setResolution(int x_res, int y_res)
{
    this->_XResolutionReg.write(this->_i2cAddress, x_res);
    this->_YResolutionReg.write(this->_i2cAddress, y_res);
}

void IQSTouchpad::setReportRate(int rate, int mode)
{
    // modes:
    //
    // 0 - Active Mode
    // 1 - Idle Touch Mode
    // 2 - Idle Mode
    // 3 - LP1 Mode
    // 4 - LP2 Mode

    switch (mode)
    {
        case 0:
            this->_ReportRateActiveModeReg.write(this->_i2cAddress, rate);
            break;
        case 1:
            this->_ReportRateIdleTouchModeReg.write(this->_i2cAddress, rate);
            break;
        case 2:
            this->_ReportRateIdleModeReg.write(this->_i2cAddress, rate);
            break;
        case 3:
            this->_ReportRateLP1ModeReg.write(this->_i2cAddress, rate);
            break;
        case 4:
            this->_ReportRateLP2ModeReg.write(this->_i2cAddress, rate);
            break;
        default:
            throw std::invalid_argument("Invalid mode");
    }
}

int IQSTouchpad::getReportRate(int mode)
{
    return -1;
}

int IQSTouchpad::getPrevCycleTime()
{
    return this->_prev_cycle_time;
}
int IQSTouchpad::getXResolution()
{
    return this->_X_resolution;
}
int IQSTouchpad::getYResolution()
{
    return this->_Y_resolution;
}
bool IQSTouchpad::getRR_MISSED()
{
    return this->_RR_MISSED;
}

Finger IQSTouchpad::getFinger(int fingerIndex)
{
    if (fingerIndex >= 5)
    {
        // invalid finger index, throw error
        throw std::invalid_argument("Invalid finger index");
    }
    // get the finger
    return this->_fingers[fingerIndex];
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

void IQSTouchpad::_initialize()
{
    // change settings
    if (this->_X_res_to_set > 0 && this->_Y_res_to_set > 0)
    {
        this->setResolution(this->_X_res_to_set, this->_Y_res_to_set);
    }

    this->_initialized = true;
}

void IQSTouchpad::update()
{
    if (this->_ready)
    {
        if (!this->_initialized)
        {
            // initialize the touchpad
            this->_initialize();
        }

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

        // number of fingers
        this->_numFingers = this->_numFingersReg.read(this->_i2cAddress);

        if (this->_numFingers > 0)
        {
            // finger 1
            this->_fingers[0].x = this->_finger1XReg.read(this->_i2cAddress);
            this->_fingers[0].y = this->_finger1YReg.read(this->_i2cAddress);
            this->_fingers[0].force = this->_finger1StrengthReg.read(this->_i2cAddress);
            this->_fingers[0].area = this->_finger1AreaReg.read(this->_i2cAddress);

            if (this->_numFingers > 1)
            {

                // finger 2
                this->_fingers[1].x = this->_finger2XReg.read(this->_i2cAddress);
                this->_fingers[1].y = this->_finger2YReg.read(this->_i2cAddress);
                this->_fingers[1].force = this->_finger2StrengthReg.read(this->_i2cAddress);
                this->_fingers[1].area = this->_finger2AreaReg.read(this->_i2cAddress);

                if (this->_numFingers > 2)
                {
                    // finger 3
                    this->_fingers[2].x = this->_finger3XReg.read(this->_i2cAddress);
                    this->_fingers[2].y = this->_finger3YReg.read(this->_i2cAddress);
                    this->_fingers[2].force = this->_finger3StrengthReg.read(this->_i2cAddress);
                    this->_fingers[2].area = this->_finger3AreaReg.read(this->_i2cAddress);

                    if (this->_numFingers > 3)
                    {
                        // finger 4
                        this->_fingers[3].x = this->_finger4XReg.read(this->_i2cAddress);
                        this->_fingers[3].y = this->_finger4YReg.read(this->_i2cAddress);
                        this->_fingers[3].force = this->_finger4StrengthReg.read(this->_i2cAddress);
                        this->_fingers[3].area = this->_finger4AreaReg.read(this->_i2cAddress);

                        if (this->_numFingers > 4)
                        {
                            // finger 5
                            this->_fingers[4].x = this->_finger5XReg.read(this->_i2cAddress);
                            this->_fingers[4].y = this->_finger5YReg.read(this->_i2cAddress);
                            this->_fingers[4].force = this->_finger5StrengthReg.read(this->_i2cAddress);
                            this->_fingers[4].area = this->_finger5AreaReg.read(this->_i2cAddress);
                        }
                    }
                }

            }
        }

        this->_prev_cycle_time = this->_PreviousCycleTimeReg.read(this->_i2cAddress);
        this->_RR_MISSED = I2CHelpers::getBit(this->_SystemInfo1Reg.read(this->_i2cAddress), 3);
        this->_X_resolution = this->_XResolutionReg.read(this->_i2cAddress);
        this->_Y_resolution = this->_YResolutionReg.read(this->_i2cAddress);

        // end communication window
        this->endCommunicationWindow();

    }

    // reset ready flag
    this->_ready = false;
}
