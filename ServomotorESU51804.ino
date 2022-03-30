/*
 * Projekt: Måling på servomotor 51804
 * Produkt: Overkørsel til modeljernbane
 * Version: 1.0
 * Type: Program
 * Programmeret af: Jan Birch
 * Opdateret: 30-03-2022
 * GNU General Public License version 3
 * This file is part of måling ESU51804.
 * 
 * Måling ESU51804 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Måling ESU51804 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with måling ESU51804.  If not, see <https://www.gnu.org/licenses/>.
 *  
 * Noter: 
 * Se målerapport for en detaljeret beskrivelse af programmet, formål og anvendelse.
 */

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards


// Common configuration
const byte motorPin = 9;
const int MinPulseWidth = 500;  //I mikrosekunder
const int MaxPulseWidth = 2400;  //I mikrosekunder

// Test for current 
const int smallCurrPulseWidth = 1000;
const int largeCurrPulseWidth = 2000;

// Test for angle
const int startAnglePW = 500;
const int endAnglePW = 1500;
const int stepAnglePW = 50;

// Control of pulswidth interval
const int startPulseWidth = 700;
const int middlePulseWidth = 1550;
const int endPulseWidth = 2300;

// Button and LED configuration
const unsigned long debounceTime = 50;
const unsigned long LEDTime = 500;
const int typePin = 2;
const int anglePin = 3;
const int typeOutPin = 13;

// Test configuration
enum {CURRENT,CTRL,ANGLE,PERIOD};
int testType;  // Strømmåling, periodetid vinkel, kontrol periodetid
const unsigned long moveTime = 15;
const unsigned long stepTime = 5000;
const unsigned long stepTimeShort = 1000;

// Script for test of current 
void testCurrent() {
  int pulseWidth; // Variable to store PulseWidth
// Go to small pulsewidth   
  for (pulseWidth = largeCurrPulseWidth; pulseWidth >= smallCurrPulseWidth; pulseWidth -= 10) { // step in 10microsekonds
    myservo.writeMicroseconds(pulseWidth);
    delay(moveTime);                       // wait for the servo to reach the position
  }
  delay(stepTime);
// Go to large pulsewidth
  for (pulseWidth = smallCurrPulseWidth; pulseWidth <= largeCurrPulseWidth; pulseWidth += 10) { // step in 10microsekonds
    myservo.writeMicroseconds(pulseWidth);
    delay(moveTime);                       // wait for the servo to reach the position
  }
  delay(stepTime);
}

// Script for test of periodtime versus angle
void testAngle() {
  int pulseWidth = startAnglePW; // Variable to store PulseWidth
  do {
    if (digitalRead(anglePin) == LOW) {
      delay(debounceTime);
      if (digitalRead(anglePin) == LOW) pulseWidth += stepAnglePW;
    }
    myservo.writeMicroseconds(pulseWidth);
    delay(stepTimeShort);
  } while (pulseWidth < endAnglePW);
  delay(stepTime);
}

// Script for control of pulswidth interval
void testPeriod() {
  int pulseWidth; // Variable to store PulseWidth
// Go to corresponding barrier up for measurement of angle
  for (pulseWidth = startPulseWidth; pulseWidth <= middlePulseWidth; pulseWidth += 10) { // step in 10microsekonds
    myservo.writeMicroseconds(pulseWidth);
    delay(moveTime);                       // wait for the servo to reach the position
  }
  delay(stepTime);
// Go to maximum PulseWidthtime for measurement of angle
  for (pulseWidth = middlePulseWidth; pulseWidth <= endPulseWidth; pulseWidth += 10) { // step in 10microsekonds
    myservo.writeMicroseconds(pulseWidth);
    delay(moveTime);                       // wait for the servo to reach the position
  }
  delay(stepTime);
// Go to adjusted start for measurement of angle
  for (pulseWidth = endPulseWidth; pulseWidth >= startPulseWidth; pulseWidth -= 10) { // step in 10microsekonds
    myservo.writeMicroseconds(pulseWidth);
    delay(moveTime);                       // wait for the servo to reach the position
  }
  delay(stepTime);
}

void LEDBlink() {
  int noLEDBlink;

  for (noLEDBlink = 0; noLEDBlink <= testType; noLEDBlink += 1) {
    digitalWrite(typeOutPin, HIGH);
    delay(LEDTime);
    digitalWrite(typeOutPin, LOW);
    delay(LEDTime);      
  }
}

void setup() {
  pinMode(typePin,INPUT_PULLUP);
  pinMode(anglePin,INPUT_PULLUP);
  pinMode(typeOutPin,OUTPUT);
  myservo.attach(motorPin, MinPulseWidth, MaxPulseWidth);  // attaches the servo on pin 9 to the servo object
//  myservo.attach(motorPin);
  testType = CURRENT;
  LEDBlink();
}

void loop() {
  switch (testType) {
    case CURRENT:
      testCurrent();
      break;
    case CTRL:
      testCurrent();
      break;
    case ANGLE:
      testAngle();
      break;
    case PERIOD:
      testPeriod();
      break;
  }
  if (digitalRead(typePin) == LOW) {
    delay(debounceTime);
    if (digitalRead(typePin) == LOW) {
      if (testType == PERIOD) testType = CURRENT;
      else testType += 1;
      LEDBlink();
    }
  }
}
