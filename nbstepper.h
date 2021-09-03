#ifndef nbstepper_h
#define nbstepper_h

#include "Arduino.h"
#include "limits.h"

class nbstepper
{
    public:
        nbstepper();
        ~nbstepper(){};
        void setup(int maxSteps, int pin1, int pin2, int pin3, int pin4, bool reverse=false, bool keepPower=false);
        void moveTo(long steps);
        void setSpeed(unsigned long speed);
        void step();
        void stop();
        bool isMoving();

    private:
        void moveMotor(int direction);

        long moveTo_;
        int maxSteps_;
        int pin1_;
        int pin2_; 
        int pin3_;
        int pin4_;
        int stepNumber_;
        unsigned long stepDelay_;
        unsigned long lastStepTime_;

        const int motorSteps_ = 4;
        bool setupComplete_;
        bool reverse_;
        bool keepPower_;

};

#endif