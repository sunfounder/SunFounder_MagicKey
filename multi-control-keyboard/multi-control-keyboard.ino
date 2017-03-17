/**********************************************************************
* Filename    : multi-control-keyboard.ino
* Description : SunFounder multi-control keyboard 驱动
* Author      : Dream
* Brand       : SunFounder
* E-mail      : service@sunfounder.com
* Website     : www.sunfounder.com
* Update      : V1.0.0    2017-3-15
*
*
* 此代码适用与SunFounder multi-control产品，用于产品模拟keyboard功能。
* 1.joystick 映射为键盘方向键
*   A->left ctrl, B->left alt, X->字母Z, Y->字母X，
*   SELECT->SPACE, START->ENTER
* 轻DIY：
*     DEBUG    等于1时，打印调试信息
*     JOYSTICK_SENSITIVITY 100  调节joystick灵敏度，范围 0 - 500
*     MINTOUCH 938      hole触摸输入的灵敏度(0-1023),越大越容易触发
*     头文件 keymap.h 中，可以更改每个按键对应的键盘映射。
**********************************************************************/
#include "Keyboard.h"
#include "keymap.h"

#define DEBUG 0          // 等于1时，打印调试信息
#define JOYSTICK_SENSITIVITY 100 // 调节joystick灵敏度，范围 0 - 500
#define MINTOUCH 938     // hole触摸输入的灵敏度(0-1023),越大越容易触发

// joystick 转换到数字量 的阈值，高于MAX，UP输入；低于MIN，DOWN输入
int MAXJOYSTICK = 1023 - JOYSTICK_SENSITIVITY;
int MINJOYSTICK = 0 + JOYSTICK_SENSITIVITY;

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
// key[0]是现在状态   statusAX
// key[1]是上一个状态 lastStatusAX
// key[2]是键值，     key value
// key[0] == 0表示键位按下
int statusSelcetor         = 0;

int statusAxisUp[]         = {0, 0, KEYBOARD_UP};
int statusAxisDown[]       = {0, 0, KEYBOARD_DOWN};
int statusAxisLeft[]       = {0, 0, KEYBOARD_LEFT};
int statusAxisRight[]      = {0, 0, KEYBOARD_RIGHT};
int statusPinUp[]          = {0, 0, KEYBOARD_UP};
int statusPinDown[]        = {0, 0, KEYBOARD_DOWN};
int statusPinLeft[]        = {0, 0, KEYBOARD_LEFT};
int statusPinRight[]       = {0, 0, KEYBOARD_RIGHT};
int statusHoleUp[]         = {0, 0, KEYBOARD_UP};
int statusHoleDown[]       = {0, 0, KEYBOARD_DOWN};
int statusHoleLeft[]       = {0, 0, KEYBOARD_LEFT};
int statusHoleRight[]      = {0, 0, KEYBOARD_RIGHT};

int statusPinA[]           = {0, 0, KEYBOARD_A};
int statusPinB[]           = {0, 0, KEYBOARD_B};
int statusPinX[]           = {0, 0, KEYBOARD_X};
int statusPinY[]           = {0, 0, KEYBOARD_Y};
int statusPinStart[]       = {0, 0, KEYBOARD_START};
int statusPinSelect[]      = {0, 0, KEYBOARD_SELECT};
int statusHoleA[]          = {0, 0, KEYBOARD_A};
int statusHoleB[]          = {0, 0, KEYBOARD_B};
int statusHoleX[]          = {0, 0, KEYBOARD_X};
int statusHoleY[]          = {0, 0, KEYBOARD_Y};
int statusHoleStart[]      = {0, 0, KEYBOARD_START};
int statusHoleSelect[]     = {0, 0, KEYBOARD_SELECT};
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
  pinMode(selector,     INPUT_PULLUP);

  //Serial.begin(9600);
  Keyboard.begin();
}

// 读值，并转换成数字状态，存入这个键的数组中
void readStatus() {
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

// 串口绘图器 Serial Plotter
void printValue(){
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
  Keyboard.press(key);
}

// Handle release command
void releaseHandle(int key) {
  Keyboard.release(key);
}

// 按键处理函数 Handle for Buttons
void keyHandle(int *key){
  if (key[1] != key[0]) {  // Status状态有变化才处理
    if (key[0] == 0)
      pressHandle(key[2]);
    else
      releaseHandle(key[2]);
    key[1] = key[0];
  }
  delay(1);
}

// 扫描各按键
void scan() {
  // Buttons
  // UP
  keyHandle(statusPinUp);
  keyHandle(statusAxisUp);
  // Down
  keyHandle(statusPinDown);
  keyHandle(statusAxisDown);
  // Left
  keyHandle(statusPinLeft);
  keyHandle(statusAxisLeft);
  // Right
  keyHandle(statusPinRight);
  keyHandle(statusAxisRight);
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
    keyHandle(statusHoleUp);
    keyHandle(statusHoleDown);
    keyHandle(statusHoleLeft);
    keyHandle(statusHoleRight);
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
