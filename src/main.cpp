#include "tone.h";

// Security points
const int door1 = 4;
const int door2 = 5;

// Miscallaneous inputs/outputs
const int unarmedLed = 2;	 // LED to light up when unarmed
const int armUnarm	 = 3;	 // button used to arm/unarm
const int spkr			 = 12;
const int armedLed	 = 13;	// LED to light up when armed

// Configuration parameters
const String pass				 = "113";	 // password
const int securityPoints = 2;			 // number of security points

const int maxWarnLevel	= 250;	// maximum warnLevel
const int warnDelayRef	= 300;	// the number to subtract warnLevel from
const int warnIncrement = 7;		// increment maxLevel by this number

// Program booleans
bool isArmed		= 1;	// armed status. set to initialize as armed or unarmed
bool soundAlarm = 0;	// should the program sound the alarm?

// Program variables
int warnLevel							 = 0;
int isOpen[securityPoints] = {	// status of security points.
		false, false};

bool waitingForPass = 0;
String readString;

unsigned long currentMillis		= millis();
unsigned long unarmIntTracker = 0;
unsigned long alarmIntTracker = 0;
bool flipflopUnarmState				= LOW;
bool flipflopAlarmState				= LOW;

// Melodies
int initMelody[]		= {FS4, GS4, AS4, CS5, DS5, FS5};
int initDurations[] = {100, 100, 100, 100, 100, 100};

int unarmMelody[]		 = {B3, B2};
int unarmDurations[] = {50, 50};

int armMelody[]		 = {C5, C6};
int armDurations[] = {50, 50};

int alarmNote = CS5;

int noteDelay = 40;

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

	playMelody(6, initMelody, initDurations, noteDelay);

	if (isArmed == 1) {
		Serial.println("Currently ARMED.");
		digitalWrite(armedLed, HIGH);
	} else {
		Serial.println("Currently UNARMED.");
		digitalWrite(unarmedLed, HIGH);
	}
}

void playMelody(int melodyLength, int *melody, int *noteDur, int noteDelay) {
	for (int i = 0; i < melodyLength; i++) {
		tone(spkr, melody[i], noteDur[i]);
		delay(noteDelay);
		noTone(spkr);
	}
}

void flipLed(int ledPin, bool &state) {
	if (state == LOW) {
		state = HIGH;
	} else {
		state = LOW;
	};

	digitalWrite(ledPin, state);
}

void loop() {
	// - Security Points -

	// door1
	if (digitalRead(door1) == LOW && isOpen[0] == false) {
		Serial.println("Door1 has opened.");
		isOpen[0] = true;
		if (isArmed == true) soundAlarm = 1;
	} else if (digitalRead(door1) == HIGH && isOpen[0] == true) {
		isOpen[0]	 = false;
		soundAlarm = 0;
		Serial.println("Door1 has closed.");
	}

	// door2
	if (digitalRead(door2) == LOW && isOpen[1] == false) {
		Serial.println("Door2 has opened.");
		isOpen[1] = true;
		if (isArmed == true) soundAlarm = 1;
	} else if (digitalRead(door2) == HIGH && isOpen[1] == true) {
		isOpen[1]	 = false;
		soundAlarm = 0;
		Serial.println("Door2 has closed.");
	}

	// - Arm/Unarm -

	if (digitalRead(armUnarm) == LOW || waitingForPass == 1) {
		if (isArmed == 1 || waitingForPass == 1) {
			if (waitingForPass == 0) {
				waitingForPass = 1;
				Serial.println("To unarm, please enter the password.");
				Serial.print("password> ");
			}

			while (Serial.available() > 0) {
				char c = Serial.read();
				readString += c;
			}

			if (readString.length() > 0) {
				Serial.println(readString);

				if (readString == pass) {
					isArmed = 0;

					playMelody(2, unarmMelody, unarmDurations, noteDelay);
					Serial.println("Successfully UNARMED.");
				} else {
					playMelody(2, armMelody, unarmDurations, noteDelay);
					Serial.println("Wrong password!");
				}
				readString		 = "";
				waitingForPass = 0;
			}

			currentMillis = millis();

			// async blink unarm led every 250ms
			if (currentMillis - unarmIntTracker >= 1000 && waitingForPass == 1) {
				unarmIntTracker = currentMillis;
				flipLed(unarmedLed, flipflopUnarmState);
			}
		} else {
			isArmed				 = 1;
			waitingForPass = 0;

			playMelody(2, armMelody, armDurations, noteDelay);
			Serial.println("Successfully ARMED.");
		}
	}

	// - Activate unarmed/armed LEDs -

	if (isArmed == 1 && waitingForPass == 0 && soundAlarm == 0) {
		digitalWrite(armedLed, HIGH);
		digitalWrite(unarmedLed, LOW);
	} else if (waitingForPass == 0 && soundAlarm == 0) {
		digitalWrite(unarmedLed, HIGH);
		digitalWrite(armedLed, LOW);
	}

	// - Sound alarm -

	if (soundAlarm == 1) {
		currentMillis = millis();

		if ((currentMillis - alarmIntTracker) >= (warnDelayRef - warnLevel)) {
			alarmIntTracker = currentMillis;

			if (warnLevel < maxWarnLevel) { warnLevel = warnLevel + warnIncrement; };

			flipLed(armedLed, flipflopAlarmState);

			if (flipflopAlarmState == HIGH) {
				tone(spkr, alarmNote);
			} else {
				noTone(spkr);
			}
		}
	} else {
		warnLevel = 0;
		noTone(spkr);
	}
}