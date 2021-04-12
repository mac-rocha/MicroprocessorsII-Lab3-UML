// included libraries
#include <LiquidCrystal.h>
#include <DS3231.h>
#include "DS1307.h"

// set constants for pins
#define powerPin 5 //pin 5 used to control fan speed
#define directionA 4  //pin 4 used to help control fan direction
#define directionB 3  //pin 3 used to help control fan direction
#define speedButton 6  //pin 6 connects button used to change fan speed
#define directionButton 2 //pin 2 connects button used to change fan direction

// global variables
int fanSpeed[] = {0, 127, 191, 255};
char* fanString[] = {"off", "1/2 speed", "3/4 speed", "full speed"};
int fanIndex = 0;
DS1307 clock;//define a object of DS1307 class

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void setup() {
  //configure clock
  clock.begin();
  
  //set pin direction
  pinMode(powerPin, OUTPUT);
  pinMode(directionA, OUTPUT);
  pinMode(directionB, OUTPUT);
  pinMode(speedButton, INPUT_PULLUP);
  pinMode(directionButton, INPUT_PULLUP);

  //set fan speed/direction at start
  analogWrite(powerPin, 0);
  digitalWrite(directionA, LOW);
  digitalWrite(directionB, HIGH);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  //configure external interrupt for direction
  attachInterrupt(digitalPinToInterrupt(directionButton), directionChange, LOW);
}

void loop() {
  // checks if speed button is pressed
  speedChange();

  // displays time, speed, and direction
  printToLCD();
}

void speedChange() {
  if (digitalRead(speedButton) == LOW) {
    delay(50);
    if (digitalRead(speedButton) == LOW) {
      fanIndex++;
      if (fanIndex > 3) {
        fanIndex = 0;
      }
      analogWrite(powerPin, fanSpeed[fanIndex]);
      delay(200);
    }
  }
}

void directionChange() {
    delay(50);
    if (digitalRead(directionButton) == LOW) {
      digitalWrite(directionA, !digitalRead(directionA));
      digitalWrite(directionB, !digitalRead(directionB));
      delay(200);
    }
}

void printToLCD() {
  printTime();
  printFan();
}

void printTime() {
  clock.getTime();
  int theHour = clock.hour;
  int theMinute = clock.minute;
  int theSecond = clock.second;
  lcd.setCursor(0,0);
  if (theHour < 10) {
    lcd.print(0);
  }
  lcd.print(theHour);
  lcd.print(":");
  if (theMinute < 10) {
    lcd.print(0);
  }
  lcd.print(theMinute);
  lcd.print(":");
  if (theSecond < 10) {
    lcd.print(0);
  }
  lcd.print(theSecond);
  lcd.print("    ");
}

void printFan() {
  lcd.setCursor(0,1);
  if (digitalRead(directionA) == HIGH) {
    lcd.print("CCW");
  }
  else {
    lcd.print("CW ");
  }

  lcd.setCursor(6,1);
  lcd.print(fanString[fanIndex]);
  lcd.print("        ");
}
