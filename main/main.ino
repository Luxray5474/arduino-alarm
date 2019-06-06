#include "Tone.h";

// Security points
const int door1 = 4;
const int door2 = 5;

// Miscallaneous inputs/outputs
const int unarmedLed = 2;       // LED to light up when unarmed
const int armUnarm = 3;         // button used to arm/unarm
const int spkr = 12;            
const int armedLed = 13;        // LED to light up when armed

// Configuration parameters
const int pass = 113;           // passkey, or PIN
const int securityPoints = 2;   // number of security points

const int maxWarnLevel = 250;   // maximum warnLevel
const int warnDelayRef = 300;   // the number to subtract warnLevel from
const int warnIncrement = 10;   // increment maxLevel by this number

// Program booleans
bool isArmed = 1;               // armed status. set to initialize as armed or unarmed
bool soundAlarm = 0;            // should the program sound the alarm?

// Program variables
int warnLevel = 0;              
int isOpen[securityPoints] = {  // status of security points. each array element
  false, false                  // refers to a security point
};

// Melodies (they only play two notes for some reason?)
int initMelody[] = {C4, E4, G4, E4, G4, C5};
int initDurations[] = {200, 100, 100, 100, 100, 100};

int unarmMelody[] = {C5, D5, E5, G5, C6};
int unarmDurations[] = {50, 50, 50, 50, 50};

int armMelody[] = {C3, D3, G3, B3, C4};
int armDurations[] = {50, 50, 50, 50, 50};

int alarmNote = CS5;

void setup() {
  Serial.begin(9600);
  
  // Security points
  pinMode(door1, INPUT_PULLUP);
  pinMode(door2, INPUT_PULLUP);

  // Misc. inputs/outputs
  pinMode(armUnarm, INPUT_PULLUP);
  pinMode(unarmedLed, OUTPUT); 
  pinMode(armedLed, OUTPUT);
  
  Serial.println("Initialized.");

  playMelody(initMelody, initDurations, 200);
  
  if(isArmed == 1) {
    Serial.println("Currently ARMED.");
    digitalWrite(armedLed, HIGH);
  } else { 
    Serial.println("Currently UNARMED.");
    digitalWrite(unarmedLed, HIGH);
  };
};

void playMelody(int melody[], int noteDur[], int noteDelay[]) {
  for(int i = 0; i < sizeof(melody); i++) {
    tone(spkr, melody[i], noteDur[i]);
    delay(noteDelay);
    noTone(spkr);
  }
}

void loop() {
  
  // - Security Points -

  // door1
  if(digitalRead(door1) == LOW && isOpen[0] == false) {
    Serial.println("Door1 has opened.");
    isOpen[0] = true;
    if(isArmed == true) soundAlarm = 1;
  } else if(digitalRead(door1) == HIGH && isOpen[0] == true) {
    isOpen[0] = false;
    soundAlarm = 0;
    Serial.println("Door1 has closed.");
  };
  
  // door2
  if(digitalRead(door2) == LOW && isOpen[1] == false) {
    Serial.println("Door2 has opened.");
    isOpen[1] = true;
    if(isArmed == true) soundAlarm = 1;
  } else if(digitalRead(door2) == HIGH && isOpen[1] == true) {
    isOpen[1] = false;
    soundAlarm = 0;
    Serial.println("Door2 has closed.");
  };

  // - Arm/Unarm - 

  if(digitalRead(armUnarm) == LOW) {
    if(isArmed == 1) {
      Serial.println("To unarm, please enter the password.");
      Serial.print("password> ");
      while(!Serial.available()) {
        digitalWrite(unarmedLed, HIGH);
        delay(250);
        digitalWrite(unarmedLed, LOW);
        delay(250);
      };
      if(Serial.available()) {
        String readString;
        while(Serial.available()) {
          char c = Serial.read();
          readString += c;
          delay(2); 
        };

        if(readString.length() > 0) {
          Serial.println(readString.toInt());
          if(readString.toInt() == pass) {
            isArmed = 0;
            playMelody(unarmMelody, unarmDurations, 50);
            Serial.println("Successfully UNARMED.");
          } else Serial.println("Wrong password!");
          readString="";
        };
      };
    } else {
      Serial.println("Successfully ARMED.");
      playMelody(armMelody, armDurations, 50);
      isArmed = 1;
    };
    delay(1000);
  };

  // - Activate unarmed/armed LEDs - 

  if(isArmed == 1) {
    digitalWrite(armedLed, HIGH);
    digitalWrite(unarmedLed, LOW);
  } else {
    digitalWrite(unarmedLed, HIGH);
    digitalWrite(armedLed, LOW);
  };

  // - Sound alarm -

  if(soundAlarm == 1) {
    if(warnLevel < maxWarnLevel) warnLevel = warnLevel + warnIncrement;
    Serial.println(warnLevel);
    digitalWrite(armedLed, HIGH);
    tone(spkr, alarmNote);
    delay(warnDelayRef - warnLevel);
    noTone(spkr);
    digitalWrite(armedLed, LOW);
    delay(warnDelayRef - warnLevel);
  } else warnLevel = 0;
}