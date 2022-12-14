#ifndef IQSQUEUE_H
#define IQSQUEUE_H

#include <functional>
#include "IQSRegisters.h"
#include <Arduino.h>

struct IQSRead
{
    int i2cAddress;
    IQSRegister* reg;
    // callback function which is given the i2c address, register address, read value as an int formatted according to the IQSRegister [see IQSRegister.h], and return(error) code, and should return void
    std::function<void(int, int, int, byte)> callback;
};

struct IQSWrite
{
    int i2cAddress;
    IQSRegister* reg;
    int valueToWrite;
    // callback function which is given the i2c address, register address, and the return(error) code, and should return void
    std::function<void(int, int, byte)> callback;
};

#endif // IQSQUEUE_H
