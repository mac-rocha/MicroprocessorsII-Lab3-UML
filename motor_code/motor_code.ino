// included libraries
#include <LiquidCrystal.h>
#include "DS1307.h"

// set constants for pins
#define powerPin 5 //pin 5 used to control fan speed
#define directionA 4  //pin 4 used to help control fan direction
#define directionB 3  //pin 3 used to help control fan direction
#define speedButton 6  //pin 6 connects button used to change fan speed
#define directionButton 2 //pin 2 connects button used to change fan direction

// global variables
int fanSpeed[] = {0, 127, 191, 255};
char* fanString[] = {"off       ", "1/2 speed ", "3/4 speed ", "full speed"};
int fanIndex = 0;
boolean doChangeDirection = false;
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

  // configure external interrupt for direction
  attachInterrupt(digitalPinToInterrupt(directionButton), setDirectionFlag, LOW);

  // configure timer interrupt
  cli();  //stop interrupts
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();//allow interrupts

}

boolean updateLCD = false;

ISR(TIMER1_COMPA_vect){  //timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  updateLCD = true;
}


void loop() {
  // controls the speed of the fan
//    speedChange();  // checks if speed button is pressed
  defaultSpeed();  // rotates the speed on a minute cycle

  // changes the direction of the fan based off of interrupt
  if (doChangeDirection) {
    doChangeDirection = false;
    directionChange();
  }

  // displays time, speed, and direction
  if (updateLCD) {
    updateLCD = false;
    printTime();
    printFan();
  }
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

void defaultSpeed() {
  if (clock.second >= 30) {
    fanIndex = 0;
  }
  else if (clock.second >= 20) {
    fanIndex = 1;
  }
  else if (clock.second >= 10) {
    fanIndex = 2;
  }
  else {
    fanIndex = 3;
  }

  analogWrite(powerPin, fanSpeed[fanIndex]);
}

void directionChange() {
    delay(50);
    if (digitalRead(directionButton) == LOW) {
      digitalWrite(directionA, !digitalRead(directionA));
      digitalWrite(directionB, !digitalRead(directionB));
      delay(500);
    }
}

void setDirectionFlag() {
  doChangeDirection = true;
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
  lcd.print("        ");
}

void printFan() {
  lcd.setCursor(0,1);
  if (digitalRead(directionA) == HIGH) {
    lcd.print("CCW   ");
  }
  else {
    lcd.print("CW    ");
  }

  lcd.setCursor(6,1);
  lcd.print(fanString[fanIndex]);
}
