/**********************************************************************
* Filename    : multi-control-midi.ino
* Description : SunFounder multi-control midi device driver
* Author      : Dream
* Brand       : SunFounder
* E-mail      : service@sunfounder.com
* Website     : www.sunfounder.com
* Update      : V2.0.0    2017-7-15
*               add pin and joystick control
*
*
* This code fits SunFounder multi-control product，which is used to simulate midi device functions.
* 1.A1,A2 for scale adjustment：A1 conducts, shift to an octave up; A2 conducts, shift to an octave down;
*                               A1 & A2 all conduct, shift to an transpose up.
* 2.A3-A10 for tones：corresponds to tone C5, D5, E5, F5, G5, A5, B5, C6(in piano keyboard mode).
* 3.Piano and drum supporting，comment out #define KEYBOARD，and uncomment //#define DRUM to shift to DRUM mode.
*
* easy-to-do DIY：
*     DEBUG    When it is 1, it will print the debugging information.
*     MINTOUCH 900      sensitivity of holes(0-1023), the larger the parameter is, the more sensitive they will be.
*     In header file notemap.h, you can modify the holes' midi tones and velocity parameter mapping.
**********************************************************************/
#define KEYBOARD         // uncomment to shift to piano keboard mode
//#define DRUM               // uncomment to shift to drum mode
#include "MIDIUSB.h"
#include "notemap.h"

#define DEBUG 1         // When it is 1, it will print the debugging information.
#define JOYSTICK_SENSITIVITY 100 // adjust the sensivity of joystick, ranging 0-500
#define MINTOUCH 938    // sensitivity of holes(0-1023), the larger the parameter is, the more sensitive they will be.
#define Y_AXIS 1        // define axis X/Y，used in joystickHandle
#define X_AXIS 0
#define debounceDelay 20

#define AxisUp      0
#define AxisLeft    1
#define AxisDown    2
#define AxisRight   3
#define PinSelect   4
#define PinStart    5
#define PinA        6
#define PinB        7
#define PinX        8
#define PinY        9
#define PinUp       10
#define PinLeft     11
#define PinDown     12
#define PinRight    13
#define HoleUp      14
#define HoleLeft    15
#define HoleDown    16
#define HoleRight   17
#define HoleSelect  18
#define HoleStart   19
#define HoleA       20
#define HoleB       21
#define HoleX       22
#define HoleY       23

// threshold of the joystick to shift to digital value, if larger than MAX, UP input; if smaller than MAX, DOWN input.
int MAXJOYSTICK = 1023 - JOYSTICK_SENSITIVITY;
int MINJOYSTICK = 0 + JOYSTICK_SENSITIVITY;
int mod = 0;                // for scale adjustment, Octave parameter

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
const int pinUp            = 16;  //Ctrl 1
const int pinLeft          = 15;  //Ctrl 2
const int pinDown          = 14;  //C
const int pinRight         = 13;  //D
const int pinSelect        = 1;   //E
const int pinStart         = 0;   //F
const int pinA             = 3;   //G
const int pinB             = 2;   //A
const int pinX             = 11;  //B
const int pinY             = 5;   //C+

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
  int cNote;     // current_note
  int lNote;     // last_note
  bool isXY;    // is x or y axis
  unsigned long dTime;  // lastDebounceTime
  bool sent;    // sent flag
};
//==============================================
// Creat an array, the member type is status_struct
status_struct  state[] = {
// pin      cStatus lStatus key  isXY dTime sent
  {joystickYAxis, 0, 0, 0,        Y_AXIS, 0, 0},
  {joystickXAxis, 0, 0, 0,        X_AXIS, 0, 0},
  {joystickYAxis, 0, 0, NOTE_C,   Y_AXIS, 0, 0},
  {joystickXAxis, 0, 0, NOTE_D,   X_AXIS, 0, 0},
  {pinSelect,     0, 0, NOTE_E,   NULL,   0, 0},
  {pinStart,      0, 0, NOTE_F,   NULL,   0, 0},
  {pinA,          0, 0, NOTE_G,   NULL,   0, 0},
  {pinB,          0, 0, NOTE_A,   NULL,   0, 0},
  {pinX,          0, 0, NOTE_B,   NULL,   0, 0},
  {pinY,          0, 0, NOTE_CP,  NULL,   0, 0},
  {pinUp,         0, 0, 0,        Y_AXIS, 0, 0},
  {pinLeft,       0, 0, 0,        X_AXIS, 0, 0},
  {pinDown,       0, 0, NOTE_C,   Y_AXIS, 0, 0},
  {pinRight,      0, 0, NOTE_D,   X_AXIS, 0, 0},
  {holeUp,        0, 0, 0,        Y_AXIS, 0, 0},
  {holeLeft,      0, 0, 0,        X_AXIS, 0, 0},
  {holeDown,      0, 0, NOTE_C,   Y_AXIS, 0, 0},
  {holeRight,     0, 0, NOTE_D,   X_AXIS, 0, 0},
  {holeSelect,    0, 0, NOTE_E,   NULL,   0, 0},
  {holeStart,     0, 0, NOTE_F,   NULL,   0, 0},
  {holeA,         0, 0, NOTE_G,   NULL,   0, 0},
  {holeB,         0, 0, NOTE_A,   NULL,   0, 0},
  {holeX,         0, 0, NOTE_B,   NULL,   0, 0},
  {holeY,         0, 0, NOTE_CP,  NULL,   0, 0}
  };
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
  controlChange(channel, CONTROL_VOLUME, 127); // setup channel
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
  else{
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

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).
void noteOn(byte chn, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | chn, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
  MidiUSB.flush();
}
void noteOff(byte chn, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | chn, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
  MidiUSB.flush();
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).
void controlChange(byte chn, byte ctrl, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | chn, ctrl, value};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

// Handle for Buttons function
void midiHandle(int num){
  if (state[num].sent) {
    if (state[num].cStatus == 0){
      debug(state[num].cStatus);debug(state[num].lStatus);
      //Serial.print(num);Serial.println(" pressed");
      noteOn(channel, (state[num].cNote + mod), currentVelocity);
      state[num].lNote = state[num].cNote + mod;
      Serial.print("noteOn  ");Serial.println(state[num].lNote);
    }
    else
      noteOff(channel, state[num].lNote, currentVelocity);
      Serial.print("      noteOff  ");Serial.println(state[num].lNote);
    state[num].sent = 0;
  }
  delay(1);
}

int controlOctave(){// A1,A2 is control scale adjustment
  int ctrl1, ctrl2;
  if (state[AxisUp].cStatus == 0  ||
      state[HoleUp].cStatus == 0 ||
      state[PinUp].cStatus == 0)
    ctrl1 = 0;
  else
    ctrl1 = 1;

  if (state[AxisLeft].cStatus == 0  ||
      state[PinLeft].cStatus == 0 ||
      state[HoleLeft].cStatus == 0)
    ctrl2 = 0;
  else
    ctrl2 = 1;

  if((ctrl1 + ctrl2) == 0) // A1 and A2 conduct, shift to an transpose up
    mod = 1;
  else if (ctrl1 == 0)  // A1 conducts, shift to an octave up
    mod = 12;
  else if (ctrl2 == 0)  // A2 conducts, shift to an octave down
    mod = -12;
  else
    mod = 0;
}

// scan all buttons state
void scan() {
  // Buttons
  if (statusMode == 0){  // Enable Hole Mode
    midiHandle(HoleDown);
    midiHandle(HoleRight);
    midiHandle(HoleA);
    midiHandle(HoleB);
    midiHandle(HoleX);
    midiHandle(HoleY);
    midiHandle(HoleStart);
    midiHandle(HoleSelect);
  }
  else {
    midiHandle(PinDown);
    midiHandle(PinRight);
    midiHandle(AxisDown);
    midiHandle(AxisRight);
    midiHandle(PinA);
    midiHandle(PinB);
    midiHandle(PinX);
    midiHandle(PinY);
    midiHandle(PinStart);
    midiHandle(PinSelect);
  }
}

void loop() {
  readStatus();
  //printValue(1,1,1); // holes, axis, pins
  scan();
  controlOctave();
}
