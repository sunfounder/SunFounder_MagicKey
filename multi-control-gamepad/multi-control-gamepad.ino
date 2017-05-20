/**********************************************************************
* Filename    : multi-control-gamepad.ino
* Description : SunFounder multi-control gamepad driver
* Author      : Dream
* Brand       : SunFounder
* E-mail      : service@sunfounder.com
* Website     : www.sunfounder.com
* Update      : V1.0.0    2017-3-15
*
*
* This code fits SunFounder multi-control product，which is used to simulate keyboard functions.
* 1.Supports joystick
* 2.Supports buttons A, B, X, Y, START, and SELECT
* Editable:
*     DEBUG    When it is 1, it will print the debugging information.
*     JOYSTICK_SENSITIVITY 100  adjust the sensitivity of joystick, ranging 0-500
*     MINTOUCH 938      sensitivity of holes(0-1023), the larger the parameter is, the more sensitive they will be.
*     In header file keymap.h, you can modify the holes' keyboard mapping.
*     If the axis X and Y is reverse, modify caliXAxis，caliYAxis
**********************************************************************/
#include "Joystick.h"
#include "keymap.h"

#define DEBUG 1         // When it is 1, it will print the debugging information.
#define JOYSTICK_SENSITIVITY 100 // adjust the sensivity of joystick, ranging 0-500
#define MINTOUCH 938    // sensitivity of holes(0-1023), the larger the parameter is, the more sensitive they will be.
#define Y_AXIS 1        // define axis X/Y，used in joystickHandle
#define X_AXIS 0
#define debounceDelay 20

#define AxisUp      0
#define AxisDown    1
#define AxisLeft    2
#define AxisRight   3
#define PinUp       4
#define PinDown     5
#define PinLeft     6
#define PinRight    7
#define HoleUp      8
#define HoleDown    9
#define HoleLeft    10
#define HoleRight   11
#define PinA        12
#define PinB        13
#define PinX        14
#define PinY        15
#define PinStart    16
#define PinSelect   17
#define HoleA       18
#define HoleB       19
#define HoleX       20
#define HoleY       21
#define HoleStart   22
#define HoleSelect  23

// threshold of the joystick to shift to digital value, if larger than MAX, UP input; if smaller than MAX, DOWN input.
int MAXJOYSTICK = 1023 - JOYSTICK_SENSITIVITY;
int MINJOYSTICK = 0 + JOYSTICK_SENSITIVITY;

int caliXAxis      = -1;  // adjust the X axis direction, value 1 or -1
int caliYAxis      = -1;  // adjust the Y axis direction, value 1 or -1
int directionStep  = 127; // joystick direction step, ranging 0~127
int JOYSTICK_UP    = caliYAxis * directionStep;
int JOYSTICK_LEFT  = caliXAxis * directionStep;
int JOYSTICK_DOWN  = -JOYSTICK_UP;
int JOYSTICK_RIGHT = -JOYSTICK_LEFT;

//==============================================
// set pin numbers for the buttons:
const int joystickXAxis    = A0;
const int joystickYAxis    = A11;
const int pinUp            = 16;
const int pinDown          = 15;
const int pinLeft          = 14;
const int pinRight         = 13;
const int pinA             = 3;
const int pinB             = 2;
const int pinX             = 11;
const int pinY             = 5;
const int pinStart         = 0;
const int pinSelect        = 1;
const int Mode             = 7;

// analog clip hole:
const int holeUp           = A1;
const int holeLeft         = A2;
const int holeDown         = A3;
const int holeRight        = A4;
const int holeSelect       = A5;
const int holeStart        = A6;
const int holeA            = A7;
const int holeB            = A8;
const int holeX            = A9;
const int holeY            = A10;
//==============================================

int statusMode = 0;// if mode on left(==0),hole key enable


//==============================================
// Creat a struct type named status_struct
struct status_struct{
  int pin;     // pin number
  int cStatus; // current_status
  int lStatus; // last_status
  int key;     // key value
  bool isXY;    // is x or y axis
  unsigned long dTime;  // lastDebounceTime
  bool sent;    // sent flag
};
//==============================================
// Creat an array, the member type is status_struct
status_struct  state[] = {
// pin      cStatus  lStatus   key       isXY dTime sent
  {joystickYAxis, 0, 0, JOYSTICK_UP,     Y_AXIS, 0, 0},
  {joystickXAxis, 0, 0, JOYSTICK_LEFT,   X_AXIS, 0, 0},
  {joystickYAxis, 0, 0, JOYSTICK_DOWN,   Y_AXIS, 0, 0},
  {joystickXAxis, 0, 0, JOYSTICK_RIGHT,  X_AXIS, 0, 0},
  {pinUp,         0, 0, JOYSTICK_UP,     Y_AXIS, 0, 0},
  {pinDown,       0, 0, JOYSTICK_DOWN,   Y_AXIS, 0, 0},
  {pinLeft,       0, 0, JOYSTICK_LEFT,   X_AXIS, 0, 0},
  {pinRight,      0, 0, JOYSTICK_RIGHT,  X_AXIS, 0, 0},
  {holeUp,        0, 0, JOYSTICK_UP,     Y_AXIS, 0, 0},
  {holeDown,      0, 0, JOYSTICK_DOWN,   Y_AXIS, 0, 0},
  {holeLeft,      0, 0, JOYSTICK_LEFT,   X_AXIS, 0, 0},
  {holeRight,     0, 0, JOYSTICK_RIGHT,  X_AXIS, 0, 0},
  {pinA,          0, 0, JOYSTICK_A,      NULL,   0, 0},
  {pinB,          0, 0, JOYSTICK_B,      NULL,   0, 0},
  {pinX,          0, 0, JOYSTICK_X,      NULL,   0, 0},
  {pinY,          0, 0, JOYSTICK_Y,      NULL,   0, 0},
  {pinStart,      0, 0, JOYSTICK_START,  NULL,   0, 0},
  {pinSelect,     0, 0, JOYSTICK_SELECT, NULL,   0, 0},
  {holeA,         0, 0, JOYSTICK_A,      NULL,   0, 0},
  {holeB,         0, 0, JOYSTICK_B,      NULL,   0, 0},
  {holeX,         0, 0, JOYSTICK_X,      NULL,   0, 0},
  {holeY,         0, 0, JOYSTICK_Y,      NULL,   0, 0},
  {holeStart,     0, 0, JOYSTICK_START,  NULL,   0, 0},
  {holeSelect,    0, 0, JOYSTICK_SELECT, NULL,   0, 0}};
//==============================================

// initialize the buttons' inputs:
void setup() {
  pinMode(pinUp,        INPUT_PULLUP);
  pinMode(pinLeft,      INPUT_PULLUP);
  pinMode(pinDown,      INPUT_PULLUP);
  pinMode(pinRight,     INPUT_PULLUP);
  pinMode(pinA,         INPUT_PULLUP);
  pinMode(pinB,         INPUT_PULLUP);
  pinMode(pinX,         INPUT_PULLUP);
  pinMode(pinY,         INPUT_PULLUP);
  pinMode(pinStart,     INPUT_PULLUP);
  pinMode(pinSelect,    INPUT_PULLUP);
  pinMode(Mode,         INPUT_PULLUP);

  Serial.begin(9600);
  Joystick.begin();
}

void debounced_read(int num){ // digitalRead and dithering elimination
  int reading = digitalRead(state[num].pin);

  if (reading != state[num].lStatus) {
    state[num].dTime = millis();
  }

  if ((millis() - state[num].dTime) > debounceDelay) {
    if (reading != state[num].cStatus) {
      state[num].cStatus = reading;
      Serial.println("have a input");
      state[num].sent = 1;
    }
  }
  state[num].lStatus = reading;
}

void analog_read(int num){ // analogRead, and shift to digital one to store
  bool threshold;
  int value      = analogRead(state[num].pin);
  if(num < 4){  // if is joystick pin
    if(num==0 || num==3)       // if is up or right
      threshold = (value > MAXJOYSTICK);
    else if (num==1 || num==2) // if is down or left
      threshold = (value < MINJOYSTICK);
  }
  else          // if is hole pin
    threshold = (value < MINTOUCH);

  if (threshold)     // the status is pressed
    state[num].cStatus    = 0;
  else               // the status is released
    state[num].cStatus    = 1;

  if (state[num].lStatus != state[num].cStatus) { // handle it when its state changes
    state[num].sent = 1;
    state[num].lStatus = state[num].cStatus;
  }
}

// read the analog, and shift to digital one to store in array
void readStatus() {
  debounced_read(PinUp);
  debounced_read(PinDown);
  debounced_read(PinLeft);
  debounced_read(PinRight);
  debounced_read(PinA);
  debounced_read(PinB);
  debounced_read(PinX);
  debounced_read(PinY);
  debounced_read(PinStart);
  debounced_read(PinSelect);

  analog_read(AxisUp);
  analog_read(AxisDown);
  analog_read(AxisLeft);
  analog_read(AxisRight);

  statusMode = digitalRead(Mode);
  if (statusMode == 0){  // Enable Hole Mode
    analog_read(HoleUp);
    analog_read(HoleLeft);
    analog_read(HoleDown);
    analog_read(HoleRight);
    analog_read(HoleA);
    analog_read(HoleB);
    analog_read(HoleX);
    analog_read(HoleY);
    analog_read(HoleStart);
    analog_read(HoleSelect);
  }
}

// Serial Plotter
void debug(int value){
  Serial.print(value);Serial.print(',');
}
void printValue(bool holes, bool axis, bool pins){
  if (holes){ // printValue holes status
    debug(state[HoleUp].cStatus);
    debug(state[HoleLeft].cStatus);
    debug(state[HoleDown].cStatus);
    debug(state[HoleRight].cStatus);
    debug(state[HoleSelect].cStatus);
    debug(state[HoleStart].cStatus);
    debug(state[HoleA].cStatus);
    debug(state[HoleB].cStatus);
    debug(state[HoleX].cStatus);
    debug(state[HoleY].cStatus);
    debug(MINTOUCH);
  }
  if (axis){ // printValue axis status
    debug(state[AxisUp].cStatus);
    debug(state[AxisDown].cStatus);
    debug(state[AxisLeft].cStatus);
    debug(state[AxisRight].cStatus);
    debug(state[AxisUp].lStatus);
    debug(state[AxisDown].lStatus);
    debug(state[AxisLeft].lStatus);
    debug(state[AxisRight].lStatus);
  }
  if (pins){ // printValue pins status
    debug(state[PinUp].cStatus);
    debug(state[PinDown].cStatus);
    debug(state[PinLeft].cStatus);
    debug(state[PinRight].cStatus);
    debug(state[PinA].cStatus);
    debug(state[PinB].cStatus);
    debug(state[PinX].cStatus);
    debug(state[PinY].cStatus);
    debug(state[PinStart].cStatus);
    debug(state[PinSelect].cStatus);
  }
  Serial.println(' ');
}

// Handle press command
void pressHandle(int key) {
  Joystick.pressButton(key);
}
// Handle release command
void releaseHandle(int key) {
  Joystick.releaseButton(key);
}

// Handle for Buttons function
void keyHandle(int num){
  if (state[num].sent) {
    if (state[num].cStatus == 0){
      debug(state[num].cStatus);debug(state[num].lStatus);
      //Serial.print(num);Serial.println(" pressed");
      pressHandle(state[num].key);
    }
    else
      releaseHandle(state[num].key);
    state[num].sent = 0;
  }
  delay(1);
}

// Handle for joystick
void joystickHandle(int num){
  // key[0] is the present state, key[1] is the previous state, key[2] is the key value
  // key[3] is axis X/Y
  // key[0] ==  means the corresponding button is activated
  if (state[num].sent) { // handle it when its state changes
    if (state[num].cStatus == 0){
      if(state[num].isXY == Y_AXIS){ // according to key[3] to select setYAxis or setXAxis
        debug(state[num].cStatus);debug(state[num].lStatus);
        Serial.println("Y axis pressed");
        Joystick.setYAxis(state[num].key);
       }
      else if(state[num].isXY == X_AXIS){
        //Serial.println("X axis pressed");
        Joystick.setXAxis(state[num].key);
       }
    }
    else{ // No direction step change detected, joystick parameter return to 0
      if(state[num].isXY == Y_AXIS){
        Serial.println("Y axis release");
        Joystick.setYAxis(0);
       }
      else if(state[num].isXY == X_AXIS){
        //Serial.println("X axis release");
        Joystick.setXAxis(0);
       }
    }
    state[num].sent = 0;
  }
  delay(1);
}

// scan all buttons state
void scan() {
  // Buttons
  joystickHandle(PinUp);
  joystickHandle(AxisUp);
  joystickHandle(PinDown);
  joystickHandle(AxisDown);
  joystickHandle(PinLeft);
  joystickHandle(AxisLeft);
  joystickHandle(PinRight);
  joystickHandle(AxisRight);
  keyHandle(PinA);
  keyHandle(PinB);
  keyHandle(PinX);
  keyHandle(PinY);
  keyHandle(PinStart);
  keyHandle(PinSelect);

  if (statusMode == 0){  // Enable Hole Mode
    joystickHandle(HoleUp);
    joystickHandle(HoleDown);
    joystickHandle(HoleLeft);
    joystickHandle(HoleRight);
    keyHandle(HoleA);
    keyHandle(HoleB);
    keyHandle(HoleX);
    keyHandle(HoleY);
    keyHandle(HoleStart);
    keyHandle(HoleSelect);
  }
}

void loop() {
  readStatus();
  //printValue(1,1,1); // holes, axis, pins
  scan();
}
