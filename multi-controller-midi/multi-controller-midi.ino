#include "MIDIUSB.h"
#include "notemap.h"

#define MINTOUCH 900
#define CHANNEL_KEYBOARD 0
#define CHANNEL_DRUM 9  // CHANNEL
#define CONTROL_VOLUME  7

int currentVelocity = 127;
int pmmin;
int pmmax;

int mod = 0;
int channel = CHANNEL_KEYBOARD;

const int selector         = 7;
const int channelPlus      = 3;
const int channelMinus     = 2;
int statusSelcetor         = 0;
//==============================================
// Set variables
// holeAX[0] 直接读取到的模拟值    valueAX
// holeAX[1] 模拟值转成数字量      statusAX
// holeAX[2] 读值有改变才进行输出  lastStatusAX
// holeAX[3] note map              NOTE_A4
// holeAX[4] lastNoteAX, 变调的值  lastNoteAX
int holeA2[]  = {0, 0, 0, 0};
int holeA3[]  = {0, 0, 0, 0};
int holeA4[]  = {0, 0, 0, NOTE_A4, 0};
int holeA5[]  = {0, 0, 0, NOTE_A5, 0};
int holeA6[]  = {0, 0, 0, NOTE_A6, 0};
int holeA7[]  = {0, 0, 0, NOTE_A7, 0};
int holeA8[]  = {0, 0, 0, NOTE_A8, 0};
int holeA9[]  = {0, 0, 0, NOTE_A9, 0};
int holeA10[] = {0, 0, 0, NOTE_A10, 0};
int holeA11[] = {0, 0, 0, NOTE_A11, 0};

//==============================================

void setup() {
  Serial.begin(115200);
  pinMode(channelPlus, INPUT_PULLUP);
  pinMode(channelMinus, INPUT_PULLUP);
  controlChange(channel, CONTROL_VOLUME, 127);
}

void readStatus(){
  //statusSelcetor = digitalRead(selector);
  //if(statusSelcetor == 0){    // hole 开关
    holeA2[0]  = analogRead(A2);
    holeA3[0]  = analogRead(A3);
    holeA4[0]  = analogRead(A4);
    holeA5[0]  = analogRead(A5);
    holeA6[0]  = analogRead(A6);
    holeA7[0]  = analogRead(A7);
    holeA8[0]  = analogRead(A8);
    holeA9[0]  = analogRead(A9);
    holeA10[0] = analogRead(A10);
    holeA11[0] = analogRead(A11);

    if (holeA2[0] < MINTOUCH)    holeA2[1]   = 0;
    else                      holeA2[1]   = 1;
    if (holeA3[0] < MINTOUCH)    holeA3[1]   = 0;
    else                      holeA3[1]   = 1;
    if (holeA4[0] < MINTOUCH)    holeA4[1]   = 0;
    else                      holeA4[1]   = 1;
    if (holeA5[0] < MINTOUCH)    holeA5[1]   = 0;
    else                      holeA5[1]   = 1;
    if (holeA6[0] < MINTOUCH)    holeA6[1]   = 0;
    else                      holeA6[1]   = 1;
    if (holeA7[0] < MINTOUCH)    holeA7[1]   = 0;
    else                      holeA7[1]   = 1;
    if (holeA8[0] < MINTOUCH)    holeA8[1]   = 0;
    else                      holeA8[1]   = 1;
    if (holeA9[0] < MINTOUCH)    holeA9[1]   = 0;
    else                      holeA9[1]   = 1;
    if (holeA10[0] < MINTOUCH)   holeA10[1]  = 0;
    else                      holeA10[1]  = 1;
    if (holeA11[0] < MINTOUCH)   holeA11[1]  = 0;
    else                      holeA11[1]  = 1;
/*  // 音量可调节
    //currentVelocity = map(valueA0, 0, 1024, 0, 127);
    int ec1 = map(valueA3, pmmin, pmmax, 0, 127);
    //Serial.print("pm :");Serial.println(valueA3);
    /*if(valueA1 < 1000){
      controlChange(0, 7, ec1);
    }
*/

/*  // 串口绘图器
    Serial.print(holeA3[0]);Serial.print(',');
    Serial.print(holeA4[0]);Serial.print(',');
    Serial.print(holeA5[0]);Serial.print(',');
    Serial.print(holeA6[0]);Serial.print(',');
    Serial.print(holeA7[0]);Serial.print(',');
    Serial.print(holeA8[0]);Serial.print(',');
    Serial.print(holeA9[0]);Serial.print(',');
    Serial.print(holeA10[0]);Serial.print(',');
    Serial.print(holeA11[0]);Serial.print(',');
*/ //}
}

void holeHandle(int *holeAX){
  if(holeAX[2] != holeAX[1]){
    if(holeAX[1] == 0){
      noteOn(channel, (holeAX[3] + mod), currentVelocity);
      holeAX[4] = holeAX[3] + mod;
      Serial.print("noteOn  ");Serial.println(holeAX[3] + mod);
    }
    else{
      noteOff(channel, holeAX[4], currentVelocity);
      Serial.print("      noteOff  ");Serial.println(holeAX[4]);
    }
    holeAX[2] = holeAX[1];
  }
  //delay(20);
}

void scan(){
  int a = digitalRead(channelPlus);
  int b = digitalRead(channelMinus);
  if(a == 0){
    while(a == 0){a = digitalRead(channelPlus);}
    if(a == 1){
        channel += 1;
        if(channel > 15){
            channel = 15;
        }
    }
  }

  if(b == 0){
    while(b == 0){b = digitalRead(channelMinus);}
    if(b == 1){
        channel -= 1;
        if(channel < 0){
            channel = 0;
        }
    }
  }
  if((holeA2[1] + holeA3[1]) == 0) // A2和A3同时按下，升1个调
    mod = 1;
  else if (holeA2[1] == 0)  // 仅A2按下，升一个八度
    mod = 12;
  else                      // 仅A3按下，降低一个八度
    mod = -12;

  // A4
  holeHandle(holeA4);
  // A5
  holeHandle(holeA5);
  // A6
  holeHandle(holeA6);
  // A7
  holeHandle(holeA7);
  // A8
  holeHandle(holeA8);
  // A9
  holeHandle(holeA9);
  // A10
  holeHandle(holeA10);
  // A11
  holeHandle(holeA11);
}

void loop() {
  readStatus();
  scan();

  Serial.print(0);Serial.print(',');
  Serial.print(MINTOUCH);Serial.print(',');
  Serial.println(1023);
}

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
