#ifndef IQS_TOUCHPAD_H
#define IQS_TOUCHPAD_H

#include <Arduino.h>
#include "IQSRegisters.h"
#include <vector>
#include "Finger.h"

#define DEFAULT_I2C_ADDRESS 0x74


class IQSTouchpad
{
    private:
        byte _i2cAddress;
        int _PIN_RDY;
        int _PIN_RST;
        int _X_res_to_set;
        int _Y_res_to_set;
        bool _initialized = false;

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
        // settings registers
        IQSRegister _XResolutionReg;
        IQSRegister _YResolutionReg;
        IQSRegister _ReportRateActiveModeReg;
        IQSRegister _ReportRateIdleTouchModeReg;
        IQSRegister _ReportRateIdleModeReg;
        IQSRegister _ReportRateLP1ModeReg;
        IQSRegister _ReportRateLP2ModeReg;
        IQSRegister _I2CTimeoutReg;
        // flag registers
        IQSRegister _SystemInfo1Reg;
        IQSRegister _SingleFingerGestureReg;
        IQSRegister _MultiFingerGestureReg;
        // misc system info registers
        IQSRegister _PreviousCycleTimeReg;

        // system info
        int _prev_cycle_time;
        int _X_resolution;
        int _Y_resolution;

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

        void _initialize();

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
