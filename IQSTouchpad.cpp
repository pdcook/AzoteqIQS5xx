#include "IQSTouchpad.h"
#include <Arduino.h>
#include <Wire.h>
#include "I2CHelpers.h"
#include "IQSRegisters.h"
#include <vector>
#include "Finger.h"
#include <queue>
#include <functional>
#include "IQSQueue.h"

std::vector<IQSTouchpad*> IQSTouchpad::_touchpads = std::vector<IQSTouchpad*>();

IQSTouchpad::IQSTouchpad(int PIN_RDY, int PIN_RST, int X_resolution, int Y_resolution, bool switch_xy_axis, bool flip_y, bool flip_x, int maxFingers, byte i2cAddress, int PIN_SDA, int PIN_SCL, bool read_cycle_time)
{
    this->_PIN_RDY = PIN_RDY;
    this->_PIN_RST = PIN_RST;
    this->_PIN_SDA = PIN_SDA;
    this->_PIN_SCL = PIN_SCL;
    this->_i2cAddress = i2cAddress;
    this->_read_cycle_time = read_cycle_time;
    this->_initialized = false;

    this->_bytes_to_read = (this->_read_cycle_time ? 45 : 44);
    this->_finger_data_buffer = new byte[this->_bytes_to_read];

    // add this touchpad to the list of touchpads
    IQSTouchpad::_touchpads.push_back(this);

    // queue settings writes
    this->setResolution(X_resolution, Y_resolution);
    this->setXYConfig0(true, switch_xy_axis, flip_y, flip_x);
    this->setMaxFingers(maxFingers);

    // set default read address
    if (this->_read_cycle_time)
    {
        this->_setDefaultReadAddress(IQSRegisters::PreviousCycleTime);
    }
    else
    {
        this->_setDefaultReadAddress(IQSRegisters::SingleFingerGestures);
    }
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

void IQSTouchpad::_setDefaultReadAddress(IQSRegister reg)
{
    this->queueWrite(IQSRegisters::DefaultReadAddress, reg.getAddress());
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

void IQSTouchpad::setMaxFingers(int max_fingers)
{
    this->queueWrite(IQSRegisters::MaxMultiTouches, max_fingers);
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

void IQSTouchpad::setReportRate(int report_rate_milliseconds, TouchpadMode mode)
{
    // modes:
    //
    //  Active Mode
    //  Idle Touch Mode
    //  Idle Mode
    //  LP1 Mode
    //  LP2 Mode

    switch (mode)
    {
        case TouchpadMode::ACTIVE:
            this->queueWrite(IQSRegisters::ActiveModeReportRate, report_rate_milliseconds);
            break;
        case TouchpadMode::IDLE_TOUCH:
            this->queueWrite(IQSRegisters::IdleTouchModeReportRate, report_rate_milliseconds);
            break;
        case TouchpadMode::IDLE:
            this->queueWrite(IQSRegisters::IdleModeReportRate, report_rate_milliseconds);
            break;
        case TouchpadMode::LP1:
            this->queueWrite(IQSRegisters::LP1ModeReportRate, report_rate_milliseconds);
            break;
        case TouchpadMode::LP2:
            this->queueWrite(IQSRegisters::LP2ModeReportRate, report_rate_milliseconds);
            break;
        default:
            break;
            //throw std::invalid_argument("Invalid mode");
    }
}

Finger IQSTouchpad::getFinger(int finger_index)
{
    if (finger_index >= 5)
    {
        // invalid finger index, throw error
        //throw std::invalid_argument("Invalid finger index");
        return Finger(-1);
    }
    // get the finger
    return this->_fingers[finger_index];
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
    Wire.setClock((uint32_t)(5*409600));

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
        if (this->_initialized)
        {
            // update all touch data
            // this must be the first thing in the communication window, since
            // it relies on using the default read address for faster communication
            // moreover, the touchpad must be initialized (the write queue
            // has been cleared at least once) so that the default read address
            // has been set
            this->_readTouchData();


            // check the queue for any pending reads, and apply all of them
            while (!this->_readQueue.empty())
            {
                Serial.println("Reading from queue");
                IQSRead read = this->_readQueue.front();

                // read the value from the register

                byte error;
                int value = read.reg.read(read.i2cAddress, error);
                read.callback(read.i2cAddress, read.reg.getAddress(), value, error);
                Serial.println("Read from queue");

                // remove the read from the queue
                this->_readQueue.pop();
            }
        }

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

        // the touchpad must clear the write queue at least once
        // before it is initialized
        if (!this->_initialized)
        {
            // set the initialized flag
            this->_initialized = true;
            // set updated flag
            this->_wasUpdated = false;
        }
        else
        {
            // set updated flag
            this->_wasUpdated = true;
        }
        // end communication window
        this->endCommunicationWindow();

        // reset ready flag
        this->_ready = false;


        // mandatory reads

        // the number of mandatory reads should be as small as possible
        // since more reads increases the minimum achievable cycle time

        /*
         * Read the following registers:
         * - Number of fingers
         *
         * - Then the data for each finger 0, 1, 2, 3, 4 (if present):
         *   - Absolute X
         *   - Absolute Y
         *   - Touch Strength
         *   - Touch Area
         *   - (Relative X is calculated from the previous Absolute X)
         *   - (Relative Y is calculated from the previous Absolute Y)
         *
         */

        // number of fingers
        //this->_numFingers = IQSRegisters::NumFingers.read(this->_i2cAddress);

        //this->_updateFingerData(this->_numFingers);

        // end communication window
        //this->endCommunicationWindow();

        // set updated flag
        //this->_wasUpdated = true;

        // reset ready flag
        //this->_ready = false;
    }
    else
    {
        // set updated flag
        this->_wasUpdated = false;
    }

}

void IQSTouchpad::_readTouchData()
{
    // perform a current address (default address) read
    //
    // if the touchpad is not initialized, this will fail
    //
    // if this->_read_cycle_time is true, then this starts at address 0x000C
    // and reads 45 bytes
    // otherwise, this starts at address 0x000D and reads 44 bytes
    //

    byte error = I2CHelpers::readFromCurrentAddress(this->_i2cAddress, this->_bytes_to_read, this->_finger_data_buffer);

    if (error != 0) { return; }

    int buffer_index = 0;

    // if this->_read_cycle_time is true, then the first byte is
    // the duration of the previous cycle in milliseconds
    this->_prev_cycle_time = (this->_read_cycle_time ? this->_finger_data_buffer[buffer_index++] : -1);

    // the next byte is the single finger gestures
    byte single_finger_gestures = this->_finger_data_buffer[buffer_index++];
    this->_TAP = I2CHelpers::getBit(single_finger_gestures, 0);
    this->_TAP_AND_HOLD = I2CHelpers::getBit(single_finger_gestures, 1);
    this->_SWIPE_X_POS = I2CHelpers::getBit(single_finger_gestures, 2);
    this->_SWIPE_X_NEG = I2CHelpers::getBit(single_finger_gestures, 3);
    this->_SWIPE_Y_POS = I2CHelpers::getBit(single_finger_gestures, 4);
    this->_SWIPE_Y_NEG = I2CHelpers::getBit(single_finger_gestures, 5);

    // the next byte is the multi finger gestures
    byte multi_finger_gestures = this->_finger_data_buffer[buffer_index++];
    this->_TWO_FINGER_TAP = I2CHelpers::getBit(multi_finger_gestures, 0);
    this->_SCROLL = I2CHelpers::getBit(multi_finger_gestures, 1);
    this->_ZOOM = I2CHelpers::getBit(multi_finger_gestures, 2);

    // the next two bytes are system info
    byte system_info_0 = this->_finger_data_buffer[buffer_index++];
    byte system_info_1 = this->_finger_data_buffer[buffer_index++];
    this->_TP_MOVEMENT = I2CHelpers::getBit(system_info_0, 0);
    this->_PALM_DETECT = I2CHelpers::getBit(system_info_0, 1);
    this->_TOO_MANY_FINGERS = I2CHelpers::getBit(system_info_0, 2);
    this->_RR_MISSED = I2CHelpers::getBit(system_info_0, 3);
    this->_SNAP_TOGGLE = I2CHelpers::getBit(system_info_0, 4);
    this->_SWITCH_STATE = I2CHelpers::getBit(system_info_0, 5);

    // the next byte is the number of fingers
    this->_numFingers = this->_finger_data_buffer[buffer_index++];

    // if the number of fingers is 0 or there are too many, we are done
    if (this->_numFingers == 0 || this->_TOO_MANY_FINGERS)
    {
        // update all fingers to be inactive
        for (int i = 0; i < 5; i++)
        {
            this->_fingers[i].update(false, 0, 0, 0, 0);
        }

        return;
    }

    // the next 4 bytes are relative X and Y for finger 1
    // since we calculate this from the previous absolute X and Y,
    // we don't need to read it
    buffer_index += 4;

    // the final chunk of the data is 7 bytes for each finger,
    // 2 each for absolute X and Y, 2 for touch strength, and one for touch area

    // update the data for each finger
    for (int i = 0; i < 5; i++)
    {
        int x = (this->_finger_data_buffer[buffer_index++] << 8) | this->_finger_data_buffer[buffer_index++];
        int y = (this->_finger_data_buffer[buffer_index++] << 8) | this->_finger_data_buffer[buffer_index++];
        int strength = (this->_finger_data_buffer[buffer_index++] << 8) | this->_finger_data_buffer[buffer_index++];
        int area = this->_finger_data_buffer[buffer_index++];
        this->_fingers[i].update(area > 0, x, y, strength, area);
    }
}

void IQSTouchpad::_updateFingerData(int num_fingers)
{
    // read the finger data in bulk
    // this is faster than reading each register individually

    // however, since the IQS550 tracks finger numbers,
    // it is possible for only one finger to be present,
    // but that finger not be finger 1.
    // we can partially circumvent this by reading the finger data
    // all the way up to the maximum number of fingers that have
    // been active at the same time since the last time there were
    // 0 fingers present.

    // Illustration:
    //
    // 1. 0 fingers present
    // 2. 1 finger present, IQS550 reports finger 1
    // 3. 2 fingers present, IQS550 reports finger 1 and finger 2
    // 4. the finger that was finger 1 is removed.
    //          there is still 1 finger present,
    //          which the IQS reports now as finger 2,
    //          with garbage data in the finger 1 registers


    // check max active fingers
    if (num_fingers == 0)
    {
        this->_maxActiveFingers = 0;
    }
    else
    {
        if (num_fingers > this->_maxActiveFingers)
        {
            this->_maxActiveFingers = num_fingers;
        }

        // read the finger data up to the max number of active fingers
        // each finger is 7 bytes

        int buffer_size = this->_maxActiveFingers * 7;
        byte buffer[buffer_size];

        IQSRegisters::Finger1AbsoluteX.read(this->_i2cAddress, buffer, 7 * this->_maxActiveFingers);

        // each finger is organized as:
        // - 2 bytes for x (unsigned int)
        // - 2 bytes for y (unsigned int)
        // - 2 byte for force (unsigned int)
        // - 1 bytes for area  (unsigned char)

        for (int fingerIndex = 0; fingerIndex < this->_maxActiveFingers; fingerIndex++)
        {
            int fingerDataIndex = fingerIndex * 7;

            // get the finger data
            int finger_x = (buffer[fingerDataIndex] << 8) | buffer[fingerDataIndex + 1];
            int finger_y = (buffer[fingerDataIndex + 2] << 8) | buffer[fingerDataIndex + 3];
            int finger_force = (buffer[fingerDataIndex + 4] << 8) | buffer[fingerDataIndex + 5];
            int finger_area = buffer[fingerDataIndex + 6];

            // we can't use the fingerIndex to tell if the finger is present
            bool finger_present = (finger_x >= 0 && finger_y >= 0 && finger_force >= 0 && finger_area >= 0);

            // update the finger
            this->_fingers[fingerIndex].update(finger_present, finger_x, finger_y, finger_force, finger_area);
        }
    }

    // set all the fingers that are definitely not present to not present
    for (int fingerIndex = num_fingers; fingerIndex < 5; fingerIndex++)
    {
        this->_fingers[fingerIndex].update(false, 0, 0, 0, 0);
    }
}
