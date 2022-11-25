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

    // add this touchpad to the list of touchpads
    IQSTouchpad::_touchpads.push_back(this);
}

void IQSTouchpad::setResolution(int x_res, int y_res)
{
    IQSRegisters::XResolution.write(this->_i2cAddress, x_res);
    IQSRegisters::YResolution.write(this->_i2cAddress, y_res);
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
            IQSRegisters::ActiveModeReportRate.write(this->_i2cAddress, rate);
            break;
        case 1:
            IQSRegisters::IdleTouchModeReportRate.write(this->_i2cAddress, rate);
            break;
        case 2:
            IQSRegisters::IdleModeReportRate.write(this->_i2cAddress, rate);
            break;
        case 3:
            IQSRegisters::LP1ModeReportRate.write(this->_i2cAddress, rate);
            break;
        case 4:
            IQSRegisters::LP2ModeReportRate.write(this->_i2cAddress, rate);
            break;
        default:
            throw std::invalid_argument("Invalid mode");
    }
}

int IQSTouchpad::getNumFingers()
{
    return this->_numFingers;
}

int IQSTouchpad::getRelX()
{
    return this->_relX;
}

int IQSTouchpad::getRelY()
{
    return this->_relY;
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
        this->_numFingers = IQSRegisters::NumFingers.read(this->_i2cAddress);

        // relative movement is only defined when a single finger is present
        if (this->_numFingers == 1)
        {
            this->_relX = IQSRegisters::Finger1RelativeX.read(this->_i2cAddress);
            this->_relY = IQSRegisters::Finger1RelativeY.read(this->_i2cAddress);
        }
        else
        {
            this->_relX = 0;
            this->_relY = 0;
        }

        // finger 1
        this->_fingers[0].x = IQSRegisters::Finger1AbsoluteX.read(this->_i2cAddress);
        this->_fingers[0].y = IQSRegisters::Finger1AbsoluteY.read(this->_i2cAddress);
        this->_fingers[0].force = IQSRegisters::Finger1TouchStrength.read(this->_i2cAddress);
        this->_fingers[0].area = IQSRegisters::Finger1TouchArea.read(this->_i2cAddress);

        // finger 2
        this->_fingers[1].x = IQSRegisters::Finger2AbsoluteX.read(this->_i2cAddress);
        this->_fingers[1].y = IQSRegisters::Finger2AbsoluteY.read(this->_i2cAddress);
        this->_fingers[1].force = IQSRegisters::Finger2TouchStrength.read(this->_i2cAddress);
        this->_fingers[1].area = IQSRegisters::Finger2TouchArea.read(this->_i2cAddress);

        // finger 3
        this->_fingers[2].x = IQSRegisters::Finger3AbsoluteX.read(this->_i2cAddress);
        this->_fingers[2].y = IQSRegisters::Finger3AbsoluteY.read(this->_i2cAddress);
        this->_fingers[2].force = IQSRegisters::Finger3TouchStrength.read(this->_i2cAddress);
        this->_fingers[2].area = IQSRegisters::Finger3TouchArea.read(this->_i2cAddress);

        // finger 4
        this->_fingers[3].x = IQSRegisters::Finger4AbsoluteX.read(this->_i2cAddress);
        this->_fingers[3].y = IQSRegisters::Finger4AbsoluteY.read(this->_i2cAddress);
        this->_fingers[3].force = IQSRegisters::Finger4TouchStrength.read(this->_i2cAddress);
        this->_fingers[3].area = IQSRegisters::Finger4TouchArea.read(this->_i2cAddress);

        // finger 5
        this->_fingers[4].x = IQSRegisters::Finger5AbsoluteX.read(this->_i2cAddress);
        this->_fingers[4].y = IQSRegisters::Finger5AbsoluteY.read(this->_i2cAddress);
        this->_fingers[4].force = IQSRegisters::Finger5TouchStrength.read(this->_i2cAddress);
        this->_fingers[4].area = IQSRegisters::Finger5TouchArea.read(this->_i2cAddress);

        this->_prev_cycle_time = IQSRegisters::PreviousCycleTime.read(this->_i2cAddress);
        this->_RR_MISSED = I2CHelpers::getBit(IQSRegisters::SystemInfo1.read(this->_i2cAddress), 3);
        this->_X_resolution = IQSRegisters::XResolution.read(this->_i2cAddress);
        this->_Y_resolution = IQSRegisters::YResolution.read(this->_i2cAddress);

        // end communication window
        this->endCommunicationWindow();

    }

    // reset ready flag
    this->_ready = false;
}
