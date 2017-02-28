#include "MIDIUSB.h"
#include "notemap.h"

#define MINTOUCH 900
int currentVelocity = 110;

//==============================================
// Set variables
// valueAX 直接读取到的模拟值
// statusAX 模拟值通过门限转成数字量
// lastStatusAX 读值有改变才进行输出
int valueA0  = 0;
int valueA1  = 0;
int valueA2  = 0;
int valueA3  = 0;
int valueA4  = 0;
int valueA5  = 0;
int valueA6  = 0;
int valueA7  = 0;
int valueA8  = 0;
int valueA9  = 0;
int valueA10 = 0;
int valueA11 = 0;

int statusA0  = 0;
int statusA1  = 0;
int statusA2  = 0;
int statusA3  = 0;
int statusA4  = 0;
int statusA5  = 0;
int statusA6  = 0;
int statusA7  = 0;
int statusA8  = 0;
int statusA9  = 0;
int statusA10 = 0;
int statusA11 = 0;

int lastStatusA0  = 0;
int lastStatusA1  = 0;
int lastStatusA2  = 0;
int lastStatusA3  = 0;
int lastStatusA4  = 0;
int lastStatusA5  = 0;
int lastStatusA6  = 0;
int lastStatusA7  = 0;
int lastStatusA8  = 0;
int lastStatusA9  = 0;
int lastStatusA10 = 0;
int lastStatusA11 = 0;
//==============================================

void setup() {
  Serial.begin(115200);
}

void readStatus(){
  valueA0  = analogRead(A0);
  valueA1  = analogRead(A1);
  valueA2  = analogRead(A2);
  valueA3  = analogRead(A3);
  valueA4  = analogRead(A4);
  valueA5  = analogRead(A5);
  valueA6  = analogRead(A6);
  valueA7  = analogRead(A7);
  valueA8  = analogRead(A8);
  valueA9  = analogRead(A9);
  valueA10 = analogRead(A10);
  valueA11 = analogRead(A11);

  //Serial.print("A10: ");Serial.print(valueA10);
 //Serial.print("    A11: ");Serial.println(valueA11);

  Serial.print(valueA4);Serial.print(',');
  Serial.print(valueA5);Serial.print(',');
  Serial.print(valueA6);Serial.print(',');
  Serial.print(valueA7);Serial.print(',');
  Serial.print(valueA8);Serial.print(',');
  Serial.print(valueA9);Serial.print(',');
  Serial.print(valueA10);Serial.print(',');
  Serial.print(valueA11);Serial.print(',');

  if (valueA4 < MINTOUCH)    statusA4   = 0;
  else                      statusA4   = 1;
  if (valueA5 < MINTOUCH)    statusA5   = 0;
  else                      statusA5   = 1;
  if (valueA6 < MINTOUCH)    statusA6   = 0;
  else                      statusA6   = 1;
  if (valueA7 < MINTOUCH)    statusA7   = 0;
  else                      statusA7   = 1;
  if (valueA8 < MINTOUCH)    statusA8   = 0;
  else                      statusA8   = 1;
  if (valueA9 < MINTOUCH)    statusA9   = 0;
  else                      statusA9   = 1;
  if (valueA10 < MINTOUCH)   statusA10  = 0;
  else                      statusA10  = 1;
  if (valueA11 < MINTOUCH)   statusA11  = 0;
  else                      statusA11  = 1;

  //currentVelocity = map(valueA0, 0, 1024, 0, 127);
  int ec1 = map(valueA1, 0, 1024, 0, 127);
  controlChange(0, 13, ec1);
}

void noteHandle(){
  // A4
  if (lastStatusA4 != statusA4) {
    if (statusA4 == 0) {
      noteOn(0, NOTE_A4, currentVelocity);
      //a4 = NOTE_A4;
    }
    else{
      noteOff(0, NOTE_A4, currentVelocity);
    }
    lastStatusA4 = statusA4;
  }
  // A5
  if (lastStatusA5 != statusA5) {
    if (statusA5 == 0) {
      noteOn(0, NOTE_A5, currentVelocity);
      //a5 = NOTE_A5;
    }
    else{
      noteOff(0, NOTE_A5, currentVelocity);
    }
    lastStatusA5 = statusA5;
  }
  // A6
  if (lastStatusA6 != statusA6) {
    if (statusA6 == 0) {
      noteOn(0, NOTE_A6, currentVelocity);
    }
    else{
      noteOff(0, NOTE_A6, currentVelocity);
    }
    lastStatusA6 = statusA6;
  }
  // A7
  if (lastStatusA7 != statusA7) {
    if (statusA7 == 0) {
      noteOn(0, NOTE_A7, currentVelocity);
    }
    else{
      noteOff(0, NOTE_A7, currentVelocity);
    }
    lastStatusA7 = statusA7;
  }
  // A8
  if (lastStatusA8 != statusA8) {
    if (statusA8 == 0) {
      noteOn(0, NOTE_A8, currentVelocity);
    }
    else{
      noteOff(0, NOTE_A8, currentVelocity);
    }
    lastStatusA8 = statusA8;
  }
  // A9
  if (lastStatusA9 != statusA9) {
    if (statusA9 == 0) {
      noteOn(0, NOTE_A9, currentVelocity);
    }
    else{
      noteOff(0, NOTE_A9, currentVelocity);
    }
    lastStatusA9 = statusA9;
  }
  // A10
  if (lastStatusA10 != statusA10) {
    if (statusA10 == 0) {
      noteOn(0, NOTE_A10, currentVelocity);
    }
    else{
      noteOff(0, NOTE_A10, currentVelocity);
    }
    lastStatusA10 = statusA10;
  }
  // A11
  if (lastStatusA11 != statusA11) {
    if (statusA11 == 0) {
      noteOn(0, NOTE_A11, currentVelocity);
    }
    else{
      noteOff(0, NOTE_A11, currentVelocity);
    }
    lastStatusA11 = statusA11;
  }
  delay(20);
}

void loop() {
  Serial.print(0);Serial.print(',');
  Serial.print(MINTOUCH);Serial.print(',');
  readStatus();
  noteHandle();
  Serial.println(1023);
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
  MidiUSB.flush();
}
void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
  MidiUSB.flush();
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).
void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}
