#include "IQSTouchpad.h"
#include <Arduino.h>
#include <Wire.h>
#include "I2CHelpers.h"
#include "IQSRegisters.h"
#include <vector>
#include "Finger.h"
#include <stdexcept>
#include <queue>
#include <functional>
#include "IQSQueue.h"

std::vector<IQSTouchpad*> IQSTouchpad::_touchpads = std::vector<IQSTouchpad*>();

IQSTouchpad::IQSTouchpad(int PIN_RDY, int PIN_RST, int X_resolution, int Y_resolution, bool switch_xy_axis, bool flip_y, bool flip_x, int maxFingers, byte i2cAddress)
{
    this->_PIN_RDY = PIN_RDY;
    this->_PIN_RST = PIN_RST;
    this->_i2cAddress = i2cAddress;
    this->_X_res_to_set = X_resolution;
    this->_Y_res_to_set = Y_resolution;

    // add this touchpad to the list of touchpads
    IQSTouchpad::_touchpads.push_back(this);

    // queue settings writes
    this->setResolution(X_resolution, Y_resolution);
    this->setXYConfig0(true, switch_xy_axis, flip_y, flip_x);
    this->setMaxFingers(maxFingers);
}

void IQSTouchpad::queueRead(IQSRead read)
{
    this->_readQueue.push(read);
}

void IQSTouchpad::queueRead(IQSRegister reg, std::function<void(int, byte)> callback)
{
    // define a lambda function that will take the i2cAddress, registerAddress, read value, and return code and pass only the read value and return code to the callback function
    auto callbackWrapper = [callback](int i2cAddress, int registerAddress, int readValue, byte returnCode)
    {
        callback(readValue, returnCode);
    };

    // create a read object and add it to the queue
    IQSRead newRead = {
        this->_i2cAddress,
        reg,
        callbackWrapper
    };

    this->_readQueue.push(newRead);
}

void IQSTouchpad::queueRead(int registerAddress, int numBytes, int dataType, std::function<void(int,int,byte)> callback)
{
    // define a lambda function that will take the i2cAddress, registerAddress, read value, and return code and pass only the read value and return code to the callback function
    auto callbackWrapper = [callback](int i2cAddress, int registerAddress, int readValue, byte returnCode)
    {
        callback(registerAddress, readValue, returnCode);
    };

    // create an IQSRegister object
    IQSRegister reg = IQSRegister(registerAddress, numBytes, "", "", dataType);

    // create a read object and add it to the queue
    IQSRead newRead = {
        this->_i2cAddress,
        reg,
        callbackWrapper
    };

    this->_readQueue.push(newRead);
}

void IQSTouchpad::queueWrite(IQSWrite write)
{
    this->_writeQueue.push(write);
}

void IQSTouchpad::queueWrite(IQSRegister reg, int value)
{

    // create a blank callback function
    auto callbackWrapper = [](int i2cAddress, int registerAddress, byte returnCode)
    {
    };

    // create a write object and add it to the queue
    IQSWrite newWrite = {
        this->_i2cAddress,
        reg,
        value,
        callbackWrapper
    };

    this->_writeQueue.push(newWrite);
}

void IQSTouchpad::queueWrite(int registerAddress, int numBytes, int value)
{
    // create an IQSRegister object
    IQSRegister reg = IQSRegister(registerAddress, numBytes, 'b', 0);

    // create a blank callback function
    auto callbackWrapper = [](int i2cAddress, int registerAddress, byte returnCode)
    {
    };

    // create a write object and add it to the queue
    IQSWrite newWrite = {
        this->_i2cAddress,
        reg,
        value,
        callbackWrapper
    };

    this->_writeQueue.push(newWrite);
}

void IQSTouchpad::queueWrite(int registerAddress, int numBytes, int value, std::function<void(int,byte)> callback)
{
    // create an IQSRegister object
    IQSRegister reg = IQSRegister(registerAddress, numBytes, 'b', 0);

    // create a wrapper callback function
    auto callbackWrapper = [callback](int i2cAddress, int registerAddress, byte returnCode)
    {
        callback(registerAddress, returnCode);
    };

    // create a write object and add it to the queue
    IQSWrite newWrite = {
        this->_i2cAddress,
        reg,
        value,
        callbackWrapper
    };

    this->_writeQueue.push(newWrite);
}

void IQSTouchpad::setResolution(int x_res, int y_res)
{
    this->queueWrite(IQSRegisters::XResolution, x_res);
    this->queueWrite(IQSRegisters::YResolution, y_res);
}

void IQSTouchpad::setXYConfig0(byte value)
{
    this->queueWrite(IQSRegisters::XYConfig0, value);
}

void IQSTouchpad::setMaxFingers(int maxFingers)
{
    this->queueWrite(IQSRegisters::MaxMultiTouches, maxFingers);
}

void IQSTouchpad::setXYConfig0(bool PALM_REJECT, bool SWITCH_XY_AXIS, bool FLIP_Y, bool FLIP_X)
{
    byte value = 0;
    value |= PALM_REJECT << 3;
    value |= SWITCH_XY_AXIS << 2;
    value |= FLIP_Y << 1;
    value |= FLIP_X << 0;

    this->setXYConfig0(value);
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
        if (digitalRead(touchpad->PIN_RDY))
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
    attachInterrupt(this->_PIN_RDY, IQSInterruptHandler, CHANGE);
}

void IQSTouchpad::endCommunicationWindow()
{
    // end communication window
    byte error = I2CHelpers::endCommunication(this->_i2cAddress);
}

void IQSTouchpad::update()
{

    if (this->_ready)
    {
        // check the queue for any pending writes, and apply all of them
        while (!this->_writeQueue.empty())
        {
            IQSWrite write = this->_writeQueue.front();

            // write the value to the register
            byte error = write.reg.write(write.i2cAddress, write.valueToWrite);
            write.callback(write.i2cAddress, write.reg.getAddress(), error);

            // remove the write from the queue
            this->_writeQueue.pop();
        }

        // check the queue for any pending reads, and apply all of them
        while (!this->_readQueue.empty())
        {
            IQSRead read = this->_readQueue.front();

            // read the value from the register

            byte error;
            int value = read.reg.read(read.i2cAddress, error);
            read.callback(read.i2cAddress, read.reg.getAddress(), value, error);

            // remove the read from the queue
            this->_readQueue.pop();
        }

        // mandatory reads

        // the number of mandatory reads should be as small as possible
        // since more reads increases the minimum achievable cycle time

        /*
         * Read the following registers:
         * - Number of fingers
         * - Relative X (if number of fingers == 1)
         * - Relative Y (if number of fingers == 1)
         *
         * - Then the data for each finger 0, 1, 2, 3, 4 (if present):
         *   - Absolute X
         *   - Absolute Y
         *   - Touch Strength
         *   - Touch Area
         *
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

        if (this->_numFingers > 0)
        {
            // finger 1
            this->_fingers[0].x = IQSRegisters::Finger1AbsoluteX.read(this->_i2cAddress);
            this->_fingers[0].y = IQSRegisters::Finger1AbsoluteY.read(this->_i2cAddress);
            this->_fingers[0].force = IQSRegisters::Finger1TouchStrength.read(this->_i2cAddress);
            this->_fingers[0].area = IQSRegisters::Finger1TouchArea.read(this->_i2cAddress);
        }
        else
        {
            this->_fingers[0].x = -1;
            this->_fingers[0].y = -1;
            this->_fingers[0].force = -1;
            this->_fingers[0].area = -1;
        }

        if (this->_numFingers > 1)
        {
            // finger 2
            this->_fingers[1].x = IQSRegisters::Finger2AbsoluteX.read(this->_i2cAddress);
            this->_fingers[1].y = IQSRegisters::Finger2AbsoluteY.read(this->_i2cAddress);
            this->_fingers[1].force = IQSRegisters::Finger2TouchStrength.read(this->_i2cAddress);
            this->_fingers[1].area = IQSRegisters::Finger2TouchArea.read(this->_i2cAddress);
        }
        else
        {
            this->_fingers[1].x = -1;
            this->_fingers[1].y = -1;
            this->_fingers[1].force = -1;
            this->_fingers[1].area = -1;
        }

        if (this->_numFingers > 2)
        {
            // finger 3
            this->_fingers[2].x = IQSRegisters::Finger3AbsoluteX.read(this->_i2cAddress);
            this->_fingers[2].y = IQSRegisters::Finger3AbsoluteY.read(this->_i2cAddress);
            this->_fingers[2].force = IQSRegisters::Finger3TouchStrength.read(this->_i2cAddress);
            this->_fingers[2].area = IQSRegisters::Finger3TouchArea.read(this->_i2cAddress);
        }
        else
        {
            this->_fingers[2].x = -1;
            this->_fingers[2].y = -1;
            this->_fingers[2].force = -1;
            this->_fingers[2].area = -1;
        }

        if (this->_numFingers > 3)
        {
            // finger 4
            this->_fingers[3].x = IQSRegisters::Finger4AbsoluteX.read(this->_i2cAddress);
            this->_fingers[3].y = IQSRegisters::Finger4AbsoluteY.read(this->_i2cAddress);
            this->_fingers[3].force = IQSRegisters::Finger4TouchStrength.read(this->_i2cAddress);
            this->_fingers[3].area = IQSRegisters::Finger4TouchArea.read(this->_i2cAddress);
        }
        else
        {
            this->_fingers[3].x = -1;
            this->_fingers[3].y = -1;
            this->_fingers[3].force = -1;
            this->_fingers[3].area = -1;
        }

        if (this->_numFingers > 4)
        {
            // finger 5
            this->_fingers[4].x = IQSRegisters::Finger5AbsoluteX.read(this->_i2cAddress);
            this->_fingers[4].y = IQSRegisters::Finger5AbsoluteY.read(this->_i2cAddress);
            this->_fingers[4].force = IQSRegisters::Finger5TouchStrength.read(this->_i2cAddress);
            this->_fingers[4].area = IQSRegisters::Finger5TouchArea.read(this->_i2cAddress);
        }
        else
        {
            this->_fingers[4].x = -1;
            this->_fingers[4].y = -1;
            this->_fingers[4].force = -1;
            this->_fingers[4].area = -1;
        }

        //this->_prev_cycle_time = IQSRegisters::PreviousCycleTime.read(this->_i2cAddress);
        //this->_RR_MISSED = I2CHelpers::getBit(IQSRegisters::SystemInfo1.read(this->_i2cAddress), 3);
        //this->_X_resolution = IQSRegisters::XResolution.read(this->_i2cAddress);
        //this->_Y_resolution = IQSRegisters::YResolution.read(this->_i2cAddress);

        this->_TAP = I2CHelpers::getBit(IQSRegisters::SingleFingerGestures.read(this->_i2cAddress), 0);

        // end communication window
        this->endCommunicationWindow();

        // set updated flag
        this->_wasUpdated = true;

        // reset ready flag
        this->_ready = false;
    }
    else
    {
        // set updated flag
        this->_wasUpdated = false;
    }

}
