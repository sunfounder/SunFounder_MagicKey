#include "Joystick.h"
#include "keymap.h"

#define DEBUG 0         // 打印调试信息
#define JOYSTICK_SENSITIVITY 100 // 调节joystick灵敏度，范围 0 - 500
#define MINTOUCH 938    // hole触摸输入的阈值
#define Y_AXIS 1        // X/Y轴标记，joystickHandle中使用
#define X_AXIS 0

// joystick 转换到数字量的阈值，高于MAX，UP输入；低于MIN，DOWN输入
int MAXJOYSTICK = 1023 -  JOYSTICK_SENSITIVITY;
int MINJOYSTICK = 0 + JOYSTICK_SENSITIVITY;
int caliXAxis      = -1;  // 调校X方向，取值1和-1
int caliYAxis      = -1;  // 调校Y方向，取值1和-1
int directionStep  = 127; // 0~127 摇杆移动步距
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
const int selector         = 7;

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

//==============================================
// Set variables
// key[0]是现在状态，key[1]是上一个状态，key[2]是键值，
// key[3]是X/Y轴标记
// key[0] == 0表示键位按下
int statusSelcetor         = 0;

int statusAxisUp[]         = {0, 0, JOYSTICK_UP,    Y_AXIS};
int statusAxisDown[]       = {0, 0, JOYSTICK_DOWN,  Y_AXIS};
int statusAxisLeft[]       = {0, 0, JOYSTICK_LEFT,  X_AXIS};
int statusAxisRight[]      = {0, 0, JOYSTICK_RIGHT, X_AXIS};
int statusPinUp[]          = {0, 0, JOYSTICK_UP,    Y_AXIS};
int statusPinDown[]        = {0, 0, JOYSTICK_DOWN,  Y_AXIS};
int statusPinLeft[]        = {0, 0, JOYSTICK_LEFT,  X_AXIS};
int statusPinRight[]       = {0, 0, JOYSTICK_RIGHT, X_AXIS};
int statusHoleUp[]         = {0, 0, JOYSTICK_UP,    Y_AXIS};
int statusHoleDown[]       = {0, 0, JOYSTICK_DOWN,  Y_AXIS};
int statusHoleLeft[]       = {0, 0, JOYSTICK_LEFT,  X_AXIS};
int statusHoleRight[]      = {0, 0, JOYSTICK_RIGHT, X_AXIS};

int statusPinA[]           = {0, 0, JOYSTICK_A};
int statusPinB[]           = {0, 0, JOYSTICK_B};
int statusPinX[]           = {0, 0, JOYSTICK_X};
int statusPinY[]           = {0, 0, JOYSTICK_Y};
int statusPinStart[]       = {0, 0, JOYSTICK_START};
int statusPinSelect[]      = {0, 0, JOYSTICK_SELECT};
int statusHoleA[]          = {0, 0, JOYSTICK_A};
int statusHoleB[]          = {0, 0, JOYSTICK_B};
int statusHoleX[]          = {0, 0, JOYSTICK_X};
int statusHoleY[]          = {0, 0, JOYSTICK_Y};
int statusHoleStart[]      = {0, 0, JOYSTICK_START};
int statusHoleSelect[]     = {0, 0, JOYSTICK_SELECT};
//==============================================

void setup() { // initialize the buttons' inputs:
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
  pinMode(selector,     INPUT_PULLUP);

  Serial.begin(9600);
  Joystick.begin();
}

void readStatus() { // 读值，并转换成数字状态，存入这个键的数组中
  int valueXAxis      = analogRead(joystickXAxis);
  int valueYAxis      = analogRead(joystickYAxis);
  statusPinUp[0]      = digitalRead(pinUp);
  statusPinDown[0]    = digitalRead(pinDown);
  statusPinLeft[0]    = digitalRead(pinLeft);
  statusPinRight[0]   = digitalRead(pinRight);
  statusPinA[0]       = digitalRead(pinA);
  statusPinB[0]       = digitalRead(pinB);
  statusPinX[0]       = digitalRead(pinX);
  statusPinY[0]       = digitalRead(pinY);
  statusPinStart[0]   = digitalRead(pinStart);
  statusPinSelect[0]  = digitalRead(pinSelect);

  Serial.print("x:");Serial.print(valueXAxis);
  Serial.print("    y:");Serial.println(valueYAxis);

  if (valueYAxis > MAXJOYSTICK)      statusAxisUp[0]      = 0;
  else                               statusAxisUp[0]      = 1;
  if (valueYAxis < MINJOYSTICK)      statusAxisDown[0]    = 0;
  else                               statusAxisDown[0]    = 1;
  if (valueXAxis < MINJOYSTICK)      statusAxisLeft[0]    = 0;
  else                               statusAxisLeft[0]    = 1;
  if (valueXAxis > MAXJOYSTICK)      statusAxisRight[0]   = 0;
  else                               statusAxisRight[0]   = 1;

  statusSelcetor = digitalRead(selector);
  if (statusSelcetor == 0){  // Enable Hole Mode
    statusHoleUp[0]     = analogRead(holeUp);
    statusHoleLeft[0]   = analogRead(holeLeft);
    statusHoleDown[0]   = analogRead(holeDown);
    statusHoleRight[0]  = analogRead(holeRight);
    statusHoleA[0]      = analogRead(holeA);
    statusHoleB[0]      = analogRead(holeB);
    statusHoleX[0]      = analogRead(holeX);
    statusHoleY[0]      = analogRead(holeY);
    statusHoleStart[0]  = analogRead(holeStart);
    statusHoleSelect[0] = analogRead(holeSelect);

    if(DEBUG){printValue();}

    if (statusHoleUp[0] < MINTOUCH)     statusHoleUp[0]     = 0;
    else                                statusHoleUp[0]     = 1;
    if (statusHoleDown[0] < MINTOUCH)   statusHoleDown[0]   = 0;
    else                                statusHoleDown[0]   = 1;
    if (statusHoleLeft[0] < MINTOUCH)   statusHoleLeft[0]   = 0;
    else                                statusHoleLeft[0]   = 1;
    if (statusHoleRight[0] < MINTOUCH)  statusHoleRight[0]  = 0;
    else                                statusHoleRight[0]  = 1;
    if (statusHoleA[0] < MINTOUCH)      statusHoleA[0]      = 0;
    else                                statusHoleA[0]      = 1;
    if (statusHoleB[0] < MINTOUCH)      statusHoleB[0]      = 0;
    else                                statusHoleB[0]      = 1;
    if (statusHoleX[0] < MINTOUCH)      statusHoleX[0]      = 0;
    else                                statusHoleX[0]      = 1;
    if (statusHoleY[0] < MINTOUCH)      statusHoleY[0]      = 0;
    else                                statusHoleY[0]      = 1;
    if (statusHoleStart[0] < MINTOUCH)  statusHoleStart[0]  = 0;
    else                                statusHoleStart[0]  = 1;
    if (statusHoleSelect[0] < MINTOUCH) statusHoleSelect[0] = 0;
    else                                statusHoleSelect[0] = 1;
  }
  if(DEBUG){printValue();}
}

void printValue(){ // 串口绘图器 Serial Plotter
  Serial.print(statusHoleUp[0]);Serial.print(',');
  Serial.print(statusHoleLeft[0]);Serial.print(',');
  Serial.print(statusHoleDown[0]);Serial.print(',');
  Serial.print(statusHoleRight[0]);Serial.print(',');
  Serial.print(statusHoleSelect[0]);Serial.print(',');
  Serial.print(statusHoleStart[0]);Serial.print(',');
  Serial.print(statusHoleA[0]);Serial.print(',');
  Serial.print(statusHoleB[0]);Serial.print(',');
  Serial.print(statusHoleX[0]);Serial.print(',');
  Serial.print(statusHoleY[0]);Serial.print(',');
  Serial.print(MINTOUCH);Serial.print(',');
}

// Handle press command
void pressHandle(int key) {
  Joystick.pressButton(key);
}
// Handle release command
void releaseHandle(int key) {
  Joystick.releaseButton(key);
}

void keyHandle(int *key){ // 按键处理函数 Handle for Buttons
  if (key[1] != key[0]) {
    if (key[0] == 0)
      pressHandle(key[2]);
    else
      releaseHandle(key[2]);
    key[1] = key[0];
  }
  delay(1);
}


void joystickHandle(int *key){ // Handle for joystick
  // key[0]是现在状态，key[1]是上一个状态，key[2]是键值，
  // key[3]是X/Y轴标记
  // key[0] == 0表示键位按下
  if (key[1] != key[0]) { // 有状态变化时处理
    if (key[0] == 0){
      if(key[3] == Y_AXIS){ // 根据key[3]来决定使用setYAxis还是setXAxis
        Joystick.setYAxis(key[2]);
       }
      else if(key[3] == X_AXIS){
        Joystick.setXAxis(key[2]);
       }
    }
    else{ // 没有方向输入时，joystick的数值回到0
      if(key[3] == Y_AXIS){
        Joystick.setYAxis(0);
       }
      else if(key[3] == X_AXIS){
        Joystick.setXAxis(0);
       }
    }
    key[1] = key[0];
  }
  delay(1);
}

void scan() { // 扫描各按键
  // Buttons
  // UP
  joystickHandle(statusPinUp);
  joystickHandle(statusAxisUp);
  // Down
  joystickHandle(statusPinDown);
  joystickHandle(statusAxisDown);
  // Left
  joystickHandle(statusPinLeft);
  joystickHandle(statusAxisLeft);
  // Right
  joystickHandle(statusPinRight);
  joystickHandle(statusAxisRight);
  // A
  keyHandle(statusPinA);
  // B
  keyHandle(statusPinB);
  // X
  keyHandle(statusPinX);
  // Y
  keyHandle(statusPinY);
  // START
  keyHandle(statusPinStart);
  // SELECT
  keyHandle(statusPinSelect);

  if (statusSelcetor == 0){  // Enable Hole Mode
    joystickHandle(statusHoleUp);
    joystickHandle(statusHoleDown);
    joystickHandle(statusHoleLeft);
    joystickHandle(statusHoleRight);
    keyHandle(statusHoleA);
    keyHandle(statusHoleB);
    keyHandle(statusHoleX);
    keyHandle(statusHoleY);
    keyHandle(statusHoleStart);
    keyHandle(statusHoleSelect);
  }
}

void loop() {
  readStatus();
  scan();
}
