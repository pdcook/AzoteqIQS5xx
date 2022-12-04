#ifndef IQS_TOUCHPAD_H
#define IQS_TOUCHPAD_H

#include <Arduino.h>
#include "IQSRegisters.h"
#include <vector>
#include "Finger.h"
#include "IQSQueue.h"
#include <queue>
#include <functional>

#define DEFAULT_I2C_ADDRESS 0x74


class IQSTouchpad
{
    private:
        byte _i2cAddress;
        int _PIN_RDY;
        int _PIN_RST;
        int _X_res_to_set;
        int _Y_res_to_set;

        // queue for pending reads
        std::queue<IQSRead> _readQueue;

        // queue for pending writes
        std::queue<IQSWrite> _writeQueue;

        int _X_resolution;
        int _Y_resolution;

        // values that are read/updated every cycle

        // relative movement, only defined when a single finger is detected
        int _relX = 0;
        int _relY = 0;

        // system info
        int _prev_cycle_time;

        // flags
        // system flags
        bool _RR_MISSED;
        bool _SWITCH_STATE;
        bool _SNAP_TOGGLE;
        bool _TOO_MANY_FINGERS;
        bool _PALM_DETECT;
        bool _TP_MOVEMENT;
        // gesture flags
        // single finger gestures
        bool _SWIPE_Y_NEG;
        bool _SWIPE_Y_POS;
        bool _SWIPE_X_NEG;
        bool _SWIPE_X_POS;
        bool _TAP_AND_HOLD;
        bool _TAP;
        // multi finger gestures
        bool _ZOOM;
        bool _SCROLL;
        bool _TWO_FINGER_TAP;

        int _numFingers;
        struct Finger _fingers[5];
        bool _wasUpdated = false;

    public:
        IQSTouchpad(int PIN_RDY, int PIN_RST, int X_resolution = -1, int Y_resolution = -1, bool switch_xy_axis = false, bool flip_y = false, bool flip_x = false,int maxFingers = 5, byte i2cAddress = DEFAULT_I2C_ADDRESS);

        // public only because the interrupt handler needs to access it
        static std::vector<IQSTouchpad*> _touchpads;
        volatile bool _ready = false;
        const volatile bool& ready = _ready;

        // public
        void begin();
        void reset();
        void endCommunicationWindow();
        void update();
        void setResolution(int X_resolution, int Y_resolution);
        void setReportRate(int reportRate, int mode);
        void setXYConfig0(byte value);
        void setXYConfig0(bool PALM_REJECT, bool SWITCH_XY_AXIS, bool FLIP_Y, bool FLIP_X);
        void setMaxFingers(int maxFingers);
        Finger getFinger(int fingerNumber);

        // queue management
        void queueRead(IQSRead read);
        // register + callback(int readValue, byte errorCode)
        void queueRead(IQSRegister reg, std::function<void(int, byte)> callback);
        // register + #bytes + callback(int registerAddress, int readValue, byte errorCode)
        void queueRead(int registerAddress, int numBytes, int dataType, std::function<void(int, int, byte)> callback);
        // register + #bytes + callback(int registerAddress, int readValue, byte errorCode), int dataType [see IQSRegister.h]
        void queueRead(int registerAddress, int numBytes, std::function<void(int, int, byte)> callback, int dataType = 1);
        void queueWrite(IQSWrite write);
        void queueWrite(IQSRegister reg, int value);
        void queueWrite(int registerAddress, int numBytes, int value);
        // register + #bytes + valueToWrite + callback(int registerAddress, byte errorCode)
        void queueWrite(int registerAddress, int numBytes, int value, std::function<void(int, byte)> callback);

        int getReportRate(int mode);

        // getters
        const bool& wasUpdated = _wasUpdated;
        const int& numFingers = _numFingers;
        // relative movement, only defined when a single finger is detected
        const int& relX = _relX;
        const int& relY = _relY;
        const int& X_resolution = _X_resolution;
        const int& Y_resolution = _Y_resolution;
        const int& prev_cycle_time = _prev_cycle_time;
        const int& I2CAddress = _i2cAddress;
        const int& PIN_RDY = _PIN_RDY;
        const int& PIN_RST = _PIN_RST;
        const bool& RR_MISSED = _RR_MISSED;
        const bool& SWITCH_STATE = _SWITCH_STATE;
        const bool& SNAP_TOGGLE = _SNAP_TOGGLE;
        const bool& TOO_MANY_FINGERS = _TOO_MANY_FINGERS;
        const bool& PALM_DETECT = _PALM_DETECT;
        const bool& TP_MOVEMENT = _TP_MOVEMENT;
        const bool& SWIPE_Y_NEG = _SWIPE_Y_NEG;
        const bool& SWIPE_Y_POS = _SWIPE_Y_POS;
        const bool& SWIPE_X_NEG = _SWIPE_X_NEG;
        const bool& SWIPE_X_POS = _SWIPE_X_POS;
        const bool& TAP_AND_HOLD = _TAP_AND_HOLD;
        const bool& TAP = _TAP;
        const bool& ZOOM = _ZOOM;
        const bool& SCROLL = _SCROLL;
        const bool& TWO_FINGER_TAP = _TWO_FINGER_TAP;
};

#endif // IQS_TOUCHPAD_H
