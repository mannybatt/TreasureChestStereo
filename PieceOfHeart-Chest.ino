



// ***************************************
// ********** Global Variables ***********
// ***************************************


//MP3
#include <DFPlayer_Mini_Mp3.h>
#include <SoftwareSerial.h>
SoftwareSerial mp3Serial(D3, D4); // RX, TX

//Chest sensor
#define switcheroo D7
const int songCount = 21;
const int fairyCount = 25;
uint16_t song = 2;
int songPicker = 0;
int fairyPicker = 0;
int donezoSongs = 0;
int donezoFairySongs = 0;
int fairyCheck = 0; // 1=checkforFairys
int didWeJustPlaySong = 0; //1=yus
int hasTheDoorClosedYet = 0;
int r = 0;
int playedSongs[songCount];
int playedFairySongs[4];

int chestState = 0;         // current state of the button
int lastChestState = 0;     // previous state of the button
int stateChanges = 0;




// ***************************************
// *************** Setup *****************
// ***************************************


void setup() {

  randomSeed(analogRead(0));

  //Fill played songs list with 0
  for (int i = 0; i < songCount; i++) {
    playedSongs[i] = 0;
  }
  for (int i = 0; i < 4; i++) {
    playedFairySongs[i] = 0;
  }

  //Initialize Serial
  Serial.begin(115200);
  Serial.println("Booting");

  //MP3
  Serial.println("Setting up software serial");
  mp3Serial.begin (9600);
  Serial.println("Setting up mp3 player");
  mp3_set_serial (mp3Serial);
  // Delay is required before accessing player. From my experience it's ~1 sec
  delay(1000);
  //mp3_set_volume (30);
  mp3_set_volume (30);

  //Pins
  pinMode(switcheroo, INPUT_PULLUP);
  delay(3000);
}




// ***************************************
// ************* Da Loop *****************
// ***************************************


void loop() {  

  fairyChecker();
  
  //Play a normal chest sound
  if (chestState == LOW && stateChanges < 5) {
        
    //Pick a song that hasn't been played yet
    while (songPicker == 0) {
      r = random(1, songCount + 1);
      for (int i = 0; i < songCount; i++) {
        //If it's already been played, repeat search
        if (playedSongs[i] == r) {
          i = songCount;
        }
        //If it made it through the list, carry on
        if (i == songCount - 1) {
          songPicker = 1;
        }
      }
    }
    songPicker = 0;

    //Play a chosen song
    Serial.print("Playing: ");
    Serial.println(r);
    //VolumeControl
    if(r == 1 || r == 3){
      uint16_t vol = 27;
      mp3_set_volume(vol);
    }
    if(r == 6 ){
      uint16_t vol = 25;
      mp3_set_volume(vol);
    }
    mp3_play_physical(r);
    didWeJustPlaySong = 1;
    playedSongs[donezoSongs] = r;
    donezoSongs++;
    //delay(4000);
    while(digitalRead(switcheroo) == LOW){
      delay(10);
    }
    uint16_t vol = 30;
    mp3_set_volume(vol);

    //Reset counters if all songs have been played once
    if (playedSongs[songCount - 1] > 0) {
      for (int i = 0; i < songCount; i++) {
        playedSongs[i] = 0;
      }
    }
  }
  else if(chestState == LOW && stateChanges == 5){
    //randomSeed(millis());
    int q = 0;
    while(fairyPicker == 0){
      q = random(22, fairyCount + 1);
      Serial.println(q);
      for (int i = 0; i < 4; i++) {
        Serial.print("  i = ");
        Serial.println(i);
        //If it's already been played, repeat search
        if (playedFairySongs[i] == q) {
          i = 4;
        }
        //If it made it through the list, carry on
        if (i == 3) {
          fairyPicker = 1;
        }
      }
    }
    fairyPicker = 0;
    
    Serial.println(q);
    if(q == 22 || q == 25){
      uint16_t vol = 25;
      mp3_set_volume(vol);
    }
    if(r == 24){
      uint16_t vol = 27;
      mp3_set_volume(vol);
    }
    mp3_play_physical(q);
    playedFairySongs[donezoFairySongs] = q;
    donezoFairySongs++;
    if(donezoFairySongs == 4){
      donezoFairySongs = 0;
      for(int i=0; i<4; i++){
        playedFairySongs[i] = 0;
      }
    }
    while(digitalRead(switcheroo) == LOW){
      delay(10);
    }
    mp3_stop();
    uint16_t vol = 30;
    mp3_set_volume(vol);
  }
  stateChanges = 0;
  
  if(digitalRead(switcheroo) == HIGH && didWeJustPlaySong == 1){
    didWeJustPlaySong = 0;
    Serial.println("STOP - Normal");
    mp3_stop();
  }
}




// ***************************************
// ********** Backbone Methods ***********
// ***************************************


void fairyChecker(){
  chestState = digitalRead(switcheroo);
  if (chestState != lastChestState) {
    stateChanges++;
    Serial.println(stateChanges);
    delay(400);
    lastChestState = chestState;
    chestState = digitalRead(switcheroo);
    if(chestState != lastChestState){
      fairyChecker();
    }
  }
  lastChestState = chestState;
}
