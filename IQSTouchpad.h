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

    public:
        IQSTouchpad(int PIN_RDY, int PIN_RST, int X_resolution = -1, int Y_resolution = -1, byte i2cAddress = DEFAULT_I2C_ADDRESS);

        // public only because the interrupt handler needs to access it
        static std::vector<IQSTouchpad*> _touchpads;
        bool _ready = false;

        // public
        void begin();
        void reset();
        void endCommunicationWindow();
        void update();
        void setResolution(int X_resolution, int Y_resolution);
        void setReportRate(int reportRate, int mode);
        int getReportRate(int mode);
        byte getI2CAddress();
        int getPIN_RDY();
        int getPIN_RST();
        bool isReady();

        int getNumFingers();
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

        // relative movement, only defined when a single finger is detected
        int getRelX();
        int getRelY();

        // system info
        int getPrevCycleTime();
        int getXResolution();
        int getYResolution();

        bool getRR_MISSED();
        bool getSWITCH_STATE();
        bool getSNAP_TOGGLE();
        bool getTOO_MANY_FINGERS();
        bool getPALM_DETECT();
        bool getTP_MOVEMENT();
        // gesture flags
        // single finger gestures
        bool getSWIPE_Y_NEG();
        bool getSWIPE_Y_POS();
        bool getSWIPE_X_NEG();
        bool getSWIPE_X_POS();
        bool getTAP_AND_HOLD();
        bool getTAP();
        // multi finger gestures
        bool getZOOM();
        bool getSCROLL();
        bool getTWO_FINGER_TAP();

};

#endif // IQS_TOUCHPAD_H
