#include <LiquidCrystal.h>
#include <IRremote.h>
#include <TimerOne.h>
// #include <SoftwareSerial.h> 

#include "nbstepper.h";

// #define USE_SERIAL

enum ControlMode {
  ir,
  joystic,
  blueTooth,
  demo
};

ControlMode currentMode = blueTooth;

// Joystick Pins
const int joyXIn = 0;
const int joyYIn = 1;
const int joyPushBtn = 2;
const int joyRangeMin = 300;
const int joyRangeMax = 900;
bool rotateMode = false;

// blutooth
// SoftwareSerial blueToothSerial(15, 14); // RX | TX 

#define BASE_CW '1'
#define BASE_ACW '2'
#define SHOLDER_CW '3'
#define SHOLDER_ACW '4'
#define ELBOW_CW '5'
#define ELBOW_ACW '6'
#define STOP 's'

//ir sensor
IRrecv irrecv(26);
decode_results results;
// unsigned long LastResultValue;

#define VOL_UP 16736925
#define VOL_DOWN 16754775
#define REWIND 16720605
#define FFORWARD 16761405
#define DOWN_BTN 16769055
#define UP_BTN 16748655
#define PLAY 16712445
#define R_NINE 16732845

//stepper motor
int STEPS =  2048;
int NEG_STEPS = -2048;
nbstepper elbow;
nbstepper base;
nbstepper sholder;

long currentSpeed = 2;
long speedSteps = 1;
long speedMax = 17;
bool paused = true;

void step()
{
  sei(); // allow ir remote during stepping
  elbow.step();
  base.step();
  sholder.step();
}

void stop()
{
  elbow.stop();
  base.stop();
  sholder.stop();
}

void setup() {
  irrecv.enableIRIn();

  pinMode(joyPushBtn, INPUT);
  digitalWrite(joyPushBtn, HIGH);
  
  elbow.setup(STEPS, 53, 51, 52, 50, true, true);
  base.setup(STEPS, 47, 45, 46, 44, false, false);
  sholder.setup(STEPS, 41, 39, 40, 38, true, true);

  elbow.setSpeed(currentSpeed);
  base.setSpeed(currentSpeed);
  sholder.setSpeed(currentSpeed);
  #ifdef USE_SERIAL
    Serial.begin(9600);
  #endif
  // Use serial3 for Bluetooth
  Serial3.begin(9600);

  Timer1.initialize(1000);
  Timer1.attachInterrupt(step);
  delay(1000);
}

void IRControl()
{
  if (irrecv.decode(&results)) 
  {
    if (results.decode_type == 3)
    {
      #ifdef USE_SERIAL
        Serial.println(results.value);
      #endif
      switch(results.value) {
        case UP_BTN:
          elbow.moveTo(STEPS);
          break;
        case DOWN_BTN:
          elbow.moveTo(NEG_STEPS);
          break;
        case REWIND:
          base.moveTo(STEPS);
          break;
        case FFORWARD:
          base.moveTo(NEG_STEPS);
          break;
        case VOL_DOWN:
          sholder.moveTo(STEPS);
          break;
        case VOL_UP:
          sholder.moveTo(NEG_STEPS);
          break;
        case PLAY:
          stop();
          break;
        case R_NINE:
          stop();
          currentMode = demo;
      }
    }
    else
    {
      stop();
    }
    
    delay(10);
    irrecv.resume(); // Receive the next value
  }
  else
  {
    delay(10);
  }  

}

void joystickControl()
{
  int xVal = analogRead(joyXIn);
  int yVal = analogRead(joyYIn);

  if(rotateMode)
  {
    if (xVal < joyRangeMin || yVal < joyRangeMin)
    {
      base.moveTo(NEG_STEPS);
    } else if (xVal > joyRangeMax || yVal > joyRangeMax)
    {
      base.moveTo(STEPS);
    } else {
      base.stop();
    }
  }
  else 
  {
    if (xVal < joyRangeMin)
    {
      elbow.moveTo(NEG_STEPS);
    } else if (xVal > joyRangeMax)
    {
      elbow.moveTo(STEPS);
    } else {
      elbow.stop();
    }

    if (yVal < joyRangeMin)
    {
      sholder.moveTo(NEG_STEPS);
    } else if (yVal > joyRangeMax)
    {
      sholder.moveTo(STEPS);
    } else {
      sholder.stop();
    }
  }

  if (!digitalRead(joyPushBtn))
  {
    rotateMode = !rotateMode;
    while(!digitalRead(joyPushBtn))
    {
      delay(10);
    }
  }

  delay(50);
}

void blueToothMode()
{
  if(Serial3.available() > 0) 
  {
    const int command = Serial3.read();
    switch(command) {
      case BASE_CW:
        base.moveTo(STEPS);
        break;
      case BASE_ACW:
        base.moveTo(NEG_STEPS);
        break;
      case ELBOW_CW:
        elbow.moveTo(STEPS);
        break;
      case ELBOW_ACW:
        elbow.moveTo(NEG_STEPS);
        break;
      case SHOLDER_CW:
        sholder.moveTo(STEPS);
        break;
      case SHOLDER_ACW:
        sholder.moveTo(NEG_STEPS);
        break;
      case STOP:
        stop();
        break;
    }
  }
  else
  {
    delay(10);
  } 
}

void demoMode()
{
  
  const int sholderMoveRatio = 4;
  const int elbowMoveRatio = 2;
  const int baseMoveRatio = 6;

  sholder.moveTo(NEG_STEPS/sholderMoveRatio);
  while(sholder.isMoving())
  {
    delay(100);
  }

  elbow.moveTo(NEG_STEPS/elbowMoveRatio);
  while(elbow.isMoving())
  {
    delay(100);
  }

  base.moveTo(NEG_STEPS/baseMoveRatio);
  while(base.isMoving())
  {
    delay(100);
  }

  elbow.moveTo(((STEPS/elbowMoveRatio)/3)*2);
  while(elbow.isMoving())
  {
    delay(100);
  }

  elbow.moveTo((STEPS/elbowMoveRatio)/3);
  sholder.moveTo(STEPS/sholderMoveRatio);
  base.moveTo(STEPS/baseMoveRatio);
  while(elbow.isMoving() || sholder.isMoving() || base.isMoving())
  {
    delay(100);
  }

  delay(3000);
}

void loop() 
{
  switch (currentMode)
  {
  case ir:
    IRControl();
    break;
  case joystic:
    joystickControl();
    break;
  case blueTooth:
    blueToothMode();
    break;
  case demo:
    demoMode();
    break;
  default:
    break;
  }
}
