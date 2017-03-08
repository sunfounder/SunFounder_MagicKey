#include "MIDIUSB.h"
#include "notemap.h"

#define DEBUG 1
#define MINTOUCH 900
#define CHANNEL_KEYBOARD 0  // CHANNEL
#define CHANNEL_DRUM 9      // CHANNEL
#define CONTROL_VOLUME  7   // 控制种类,7表示更改channel值 Continuous Controllers
// 更多midi控制参数 http://nickfever.com/music/midi-cc-list

int currentVelocity = 127;  // 音符力度0~127 Velocity parameter

int mod = 0;                // 音阶升降参数 Octave parameter

int channel = CHANNEL_KEYBOARD;

const int selector         = 7;
const int channelPlus      = 3;  // pin A
const int channelMinus     = 2;  // pin B
int statusSelcetor         = 0;
//==============================================
// Set variables
// holeAX[0] 直接读取到的模拟值    valueAX
// holeAX[1] 模拟值转成数字量      statusAX
// holeAX[2] 读值有改变才进行输出  lastStatusAX
// holeAX[3] note map              NOTE_A4
// holeAX[4] lastNoteAX, 变调的值  lastNoteAX
int holeA1[] = {0, 0, 0, 0};
int holeA2[] = {0, 0, 0, 0};
int holeC[]  = {0, 0, 0, NOTE_C, 0};
int holeD[]  = {0, 0, 0, NOTE_D, 0};
int holeE[]  = {0, 0, 0, NOTE_E, 0};
int holeF[]  = {0, 0, 0, NOTE_F, 0};
int holeG[]  = {0, 0, 0, NOTE_G, 0};
int holeA[]  = {0, 0, 0, NOTE_A, 0};
int holeB[]  = {0, 0, 0, NOTE_B, 0};
int holeCP[] = {0, 0, 0, NOTE_CP, 0};

//==============================================

void setup() {
  Serial.begin(115200);
  pinMode(channelPlus, INPUT_PULLUP);
  pinMode(channelMinus, INPUT_PULLUP);
  controlChange(channel, CONTROL_VOLUME, 127); // 切换channel
}

void readStatus(){
  statusSelcetor = digitalRead(selector);
  if(statusSelcetor == 0){    // hole 开关
    holeA1[0]  = analogRead(A1);
    holeA2[0]  = analogRead(A2);
    holeC[0]   = analogRead(A3);
    holeD[0]   = analogRead(A4);
    holeE[0]   = analogRead(A5);
    holeF[0]   = analogRead(A6);
    holeG[0]   = analogRead(A7);
    holeA[0]   = analogRead(A8);
    holeB[0]   = analogRead(A9);
    holeCP[0]  = analogRead(A10);

    if (holeA1[0] < MINTOUCH)    holeA1[1]  = 0;
    else                      holeA1[1]   = 1;
    if (holeA2[0] < MINTOUCH)    holeA2[1]   = 0;
    else                      holeA2[1]   = 1;
    if (holeC[0] < MINTOUCH)    holeC[1]   = 0;
    else                      holeC[1]   = 1;
    if (holeD[0] < MINTOUCH)    holeD[1]   = 0;
    else                      holeD[1]   = 1;
    if (holeE[0] < MINTOUCH)    holeE[1]   = 0;
    else                      holeE[1]   = 1;
    if (holeF[0] < MINTOUCH)    holeF[1]   = 0;
    else                      holeF[1]   = 1;
    if (holeG[0] < MINTOUCH)    holeG[1]   = 0;
    else                      holeG[1]   = 1;
    if (holeA[0] < MINTOUCH)    holeA[1]   = 0;
    else                      holeA[1]   = 1;
    if (holeB[0] < MINTOUCH)    holeB[1]   = 0;
    else                      holeB[1]   = 1;
    if (holeCP[0] < MINTOUCH)   holeCP[1]  = 0;
    else                      holeCP[1]  = 1;
}
  if(DEBUG){printValue();}
}

void printValue(){  // 串口绘图器
  Serial.print(holeA1[0]);Serial.print(',');
  Serial.print(holeA2[0]);Serial.print(',');
  Serial.print(holeC[0]);Serial.print(',');
  Serial.print(holeD[0]);Serial.print(',');
  Serial.print(holeE[0]);Serial.print(',');
  Serial.print(holeF[0]);Serial.print(',');
  Serial.print(holeG[0]);Serial.print(',');
  Serial.print(holeA[0]);Serial.print(',');
  Serial.print(holeB[0]);Serial.print(',');
  Serial.print(holeCP[0]);Serial.print(',');
  Serial.print(0);Serial.print(',');
  Serial.print(MINTOUCH);Serial.print(',');
  Serial.println(1023);
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

int channelDebug(){// 调试channel, 调试得到drum channel是9，keyboard是0
  int a = digitalRead(channelPlus);
  int b = digitalRead(channelMinus);
  int channel = 0;
  if(a == 0){  // 加channel
    while(a == 0){a = digitalRead(channelPlus);}
    if(a == 1){
      channel += 1;
      if(channel > 15){channel = 15;}
    }
  }

  if(b == 0){  // 减channel
    while(b == 0){b = digitalRead(channelMinus);}
    if(b == 1){
        channel -= 1;
        if(channel < 0){channel = 0;}
    }
  }
  return channel;
}

int controlOctave(){// A1,A2控制，升高或降低音阶八度
  if((holeA1[1] + holeA2[1]) == 0) // A1和A2同时按下，升1个调 transpose up
    mod = 1;
  else if (holeA1[1] == 0)  // 仅A1按下，升一个八度 octave up
    mod = 12;
  else if (holeA2[1] == 0)  // 仅A2按下，降低一个八度 octave down
    mod = -12;
  else
    mod = 0;
  return mod;
}

void scan(){
 /*// 调试channel
  channelDebug();
 */
  // A1 A2
  mod = controlOctave();
  // C
  holeHandle(holeC);
  // D
  holeHandle(holeD);
  // E
  holeHandle(holeE);
  // F
  holeHandle(holeF);
  // G
  holeHandle(holeG);
  // A
  holeHandle(holeA);
  // B
  holeHandle(holeB);
  // CP
  holeHandle(holeCP);
}

void loop() {
  readStatus();
  scan();
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
