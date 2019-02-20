/*
  Menu driven control of a sound board over UART.
  Commands for playing by # or by name (full 11-char name)
  Hard reset and List files (when not playing audio)
  Vol + and - (only when not playing audio)
  Pause, unpause, quit playing (when playing audio)
  Current play time, and bytes remaining & total bytes (when playing audio)

  Connect UG to ground to have the sound board boot into UART mode
*/

/*


4 - white noise - 8 seconds

8 - i know you don’t like this sound - 17 seconds

5 - take out your wallet right now -1 second

6 - insert coin to disengage - 1 second


0 - fact - 10 sec

1 - fact - 10 sec

2 - fact - 10 sec

3 - fact - 10 sec

7 - fact - 10 sec
*/

#include <SoftwareSerial.h>
#include "Adafruit_Soundboard.h"

#define flex_sensor A0
#define IR_sensor A1

#define SFX_TX 5
#define SFX_RX 6
#define SFX_RST 4

# define average_len 5

// we'll be using software serial
SoftwareSerial ss = SoftwareSerial(SFX_TX, SFX_RX);
uint8_t currentSong = 0;
int n_songs = 5;
unsigned long endplaying;
bool playing = false;

int history_vals[average_len];
int index = 0;

// pass the software serial to Adafruit_soundboard, the second
// argument is the debug port (not used really) and the third
// arg is the reset pin
Adafruit_Soundboard sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);

void setup() {
  // print serial
  Serial.begin(115200);
  Serial.println("Adafruit Sound Board!");
  // soundboard serial
  ss.begin(9600);
  if (!sfx.reset()) {
    Serial.println("Not found");
    while (1);
  }
  Serial.println("SFX board found");

  for(int i = 0; i < average_len; i++){
    history_vals[i] = 20000;
  }
}


void loop() {
  int flex_val = analogRead(flex_sensor);
//
//  Serial.println(flex_val);
  //Flex Value
  if (flex_val <= 595) {
    Serial.println("FLEX DETECTED");
    currentSong += 1;

    Serial.print("Playing Track: ");
    Serial.print(currentSong-1);

    playNumber(currentSong-1, 8000);

    if (currentSong == n_songs) {
      currentSong = 0;
    }
  }

  // we only care about distance if the track isn't going
  if (millis() > endplaying){
    int distance = analogRead(IR_sensor);

    int total = 0;
    history_vals[index] = distance;
    for (int i = 0; i < average_len; i++){
      total += history_vals[i];
    }
    distance = total/average_len;
    index ++;
    if (index == average_len){
      index = 0;
    }
   
    int prev_distance = distance;

    Serial.println(distance);
    
    // distance
    if (distance < 110) {
      Serial.println("DISTANCE TRIGGER");
      playNumber(4, 8000);

      if (distance > prev_distance) {
        sfx.volUp();
        Serial.println("Insert Coin to Disengage");
        playNumber(6, 1000);
      } else if (distance < prev_distance) {
        sfx.volDown();
        Serial.println("I know you don't like this sound");
        playNumber(8, 17000);
        }
     }
  }
}

void playNumber(uint8_t number, int track_time){
  endplaying = millis() + track_time;
  sfx.playTrack((number));
}
