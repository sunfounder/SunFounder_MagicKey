/**********************************************************************
* Filename    : multi-control-midi.ino
* Description : SunFounder multi-control midi device driver
* Author      : Dream
* Brand       : SunFounder
* E-mail      : service@sunfounder.com
* Website     : www.sunfounder.com
* Update      : V1.0.0    2017-3-15
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

#define DEBUG 0             // When it is 1, it will print the debugging information
#define MINTOUCH 900        // sensitivity of holes(0-1023), the larger the parameter is, the more sensitive they will be

int mod = 0;                // for scale adjustment, Octave parameter

const int Mode             = 7;  // Switch, to turn on/off the touching holes
const int channelPlus      = 3;  // button A, used to configure channel function
const int channelMinus     = 2;  // button B, used to configure channel function
int statusMode             = 0;
//==============================================
// Set variables
// holeAX[0] the analog value to be read            valueAX
// holeAX[1] analog to digital one                  statusAX
// holeAX[2] output the value only when it changes  lastStatusAX
// holeAX[3] note map                               NOTE_AX
// holeAX[4] lastNoteAX, value of modified tone     lastNoteAX
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
  controlChange(channel, CONTROL_VOLUME, 127); // setup channel
}

void readStatus(){   // read the analog, and shift to digital one to store in array holeX[1]
  statusMode = digitalRead(Mode);
  //if(statusMode == 0){    // hole switch
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
//}
  if(DEBUG){printValue();}
}

void printValue(){  // Serial Plotter
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

void holeHandle(int *holeAX){  // handle for hole function
  if(holeAX[2] != holeAX[1]){  // Status change
    if(holeAX[1] == 0){   // one hole conducts, produce corresponding sound
      noteOn(channel, (holeAX[3] + mod), currentVelocity);
      holeAX[4] = holeAX[3] + mod;
      Serial.print("noteOn  ");Serial.println(holeAX[3] + mod);
    }
    else{  // hole conducts, no sound is produced
      noteOff(channel, holeAX[4], currentVelocity);
      Serial.print("      noteOff  ");Serial.println(holeAX[4]);
    }
    holeAX[2] = holeAX[1];
  }
  //delay(20);
}

int channelDebug(){// configure channel
  // press A once, add 1 to channel value, press B once, take 1 from channel value
  // after configuration, we know keyboard channel is 0, drum channel is 9.
  int a = digitalRead(channelPlus);
  int b = digitalRead(channelMinus);
  int channel = 0;
  if(a == 0){  // add to channel
    while(a == 0){a = digitalRead(channelPlus);}
    if(a == 1){
      channel += 1;
      if(channel > 15){channel = 15;}
    }
  }

  if(b == 0){  // take from channel
    while(b == 0){b = digitalRead(channelMinus);}
    if(b == 1){
        channel -= 1;
        if(channel < 0){channel = 0;}
    }
  }
  return channel;
}

int controlOctave(){// A1,A2 is control scale adjustment
  if((holeA1[1] + holeA2[1]) == 0) // A1 and A2 conduct, shift to an transpose up
    mod = 1;
  else if (holeA1[1] == 0)  // A1 conducts, shift to an octave up
    mod = 12;
  else if (holeA2[1] == 0)  // A2 conducts, shift to an octave down
    mod = -12;
  else
    mod = 0;
  return mod;
}

void scan(){ // scan all the buttons status
 /*// configure channel
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
