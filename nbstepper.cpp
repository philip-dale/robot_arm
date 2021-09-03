#include "nbstepper.h"
#include <TimerOne.h>

nbstepper::nbstepper()
{
    setupComplete_ = false;
    reverse_ = false;
}

void nbstepper::setup(int maxSteps, int pin1, int pin2, int pin3, int pin4, bool reverse, bool keepPower)
{
    moveTo_ = 0;
    stepNumber_ = 0;
    lastStepTime_ = 0;

    keepPower_ = keepPower;
    reverse_ = reverse;
    maxSteps_ = maxSteps;
    pin1_ = pin1;
    pin2_ = pin2;
    pin3_ = pin3;
    pin4_ = pin4;

    pinMode(pin1_, OUTPUT);
    pinMode(pin2_, OUTPUT);
    pinMode(pin3_, OUTPUT);
    pinMode(pin4_, OUTPUT);
    
    setSpeed(1);
    setupComplete_ = true;
}

void nbstepper::setSpeed(unsigned long speed)
{
    stepDelay_ = 60L * 1000L * 1000L / maxSteps_ / speed;
}

void nbstepper::moveTo(long steps)
{
    moveTo_ = steps;
}

void nbstepper::step()
{
    if (setupComplete_ && moveTo_ != 0)
    {
        unsigned long timeNow = micros();
        if (timeNow - lastStepTime_ >= stepDelay_)
        {
            if (moveTo_ > 0)
            {
                moveMotor(1);
                moveTo_--;
            } 
            else if (moveTo_ < 0)
            {
                moveMotor(-1);
                moveTo_++;
            }
            lastStepTime_ = timeNow;
            if(moveTo_ == 0)
            {
                stop();
            }
        }
    }
}

void nbstepper::moveMotor(int direction)
{
    if(!reverse_)
    {
        stepNumber_ -= direction;
    }
    else
    {
        stepNumber_ += direction;
    }
    
    if (stepNumber_ >= motorSteps_)
    {
        stepNumber_ = 0;
    }
    else if (stepNumber_ < 0)
    {
        stepNumber_ = 3;
    }
    
    switch (stepNumber_) {
        case 0:  // 1010
            digitalWrite(pin1_, HIGH);
            digitalWrite(pin2_, LOW);
            digitalWrite(pin3_, HIGH);
            digitalWrite(pin4_, LOW);
        break;
        case 1:  // 0110
            digitalWrite(pin1_, LOW);
            digitalWrite(pin2_, HIGH);
            digitalWrite(pin3_, HIGH);
            digitalWrite(pin4_, LOW);
        break;
        case 2:  //0101
            digitalWrite(pin1_, LOW);
            digitalWrite(pin2_, HIGH);
            digitalWrite(pin3_, LOW);
            digitalWrite(pin4_, HIGH);
        break;
        case 3:  //1001
            digitalWrite(pin1_, HIGH);
            digitalWrite(pin2_, LOW);
            digitalWrite(pin3_, LOW);
            digitalWrite(pin4_, HIGH);
        break;
    }
}

void nbstepper::stop()
{
    moveTo_ = 0;
    if(!keepPower_) 
    {
        digitalWrite(pin1_, LOW);
        digitalWrite(pin2_, LOW);
        digitalWrite(pin3_, LOW);
        digitalWrite(pin4_, LOW);
    }
}

bool nbstepper::isMoving()
{
    return moveTo_ != 0;
}