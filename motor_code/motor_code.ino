// included libraries
#include <LiquidCrystal.h>
#include "DS1307.h"

// set constants for pins
#define powerPin 5 //pin 5 used as output to control fan speed
#define directionA 4  //pin 4 used as output to help control fan direction
#define directionB 3  //pin 3 used as output to help control fan direction
#define speedButton 6  //pin 6 connects button used to change fan speed - used as input
#define directionButton 2 //pin 2 connects button used to change fan direction - used as input

// global variables
// variables relating to fan speed
const int fanSpeed[] = {0, 127, 191, 255};  // fan speeds
const char* fanString[] = {"off       ", "1/2 speed ", "3/4 speed ", "full speed"};  // corresponding text for fan speeds
int fanIndex = 0;  // keeps track of which iteration the fanSpeed array is on
const int fanSpeedSize = sizeof(fanSpeed) / sizeof(*fanSpeed);
// flags and object
boolean doChangeDirection = false;  // flag for interrupt to alert main code to change fan direction
boolean updateLCD = false;  // flag for timer interrupt to alert code to update LCD display
boolean turnFanOn = false;
DS1307 clock;  //defines an object of DS1307 class

////////////////////////////////////////////////////////////////////////////////////////////////////////
// determines whether the fan runs on a timer [presentation mode] or a button press [test mode]
const boolean AUTOMODE = false;
////////////////////////////////////////////////////////////////////////////////////////////////////////

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
  analogWrite(powerPin, 0);  // fan starts off
  digitalWrite(directionA, LOW);  //fan starts in CW mode
  digitalWrite(directionB, HIGH);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // configure external interrupt for direction
  attachInterrupt(digitalPinToInterrupt(directionButton), setDirectionFlag, LOW);

  // configure timer interrupt; code from
  // https://www.instructables.com/Arduino-Timer-Interrupts/
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

ISR(TIMER1_COMPA_vect){  //timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  updateLCD = true;
}


void loop() {
  // get time
  clock.getTime();
  
  // controls the speed of the fan
  if (AUTOMODE) {
    defaultSpeed();  // in this mode, the fan operates on a minute cycle
  }
  else {
    speedChange();  // in this mode, the fan operates based on the press of a button
  }

  // changes the direction of the fan based off of interrupt
  if (doChangeDirection) {
    doChangeDirection = false;  // reset flag to false
    directionChange();  // changes fan's direction
  }

  // displays time, speed, and direction
  if (updateLCD) {
    updateLCD = false;  // reset flag to false
    printTime();  // displays time on LCD
    printFan();  // displays fan direction and speed on LCD
  }
}

void speedChange() {
  if (digitalRead(speedButton) == LOW) {  // checks for button press
    delay(50);
    if (digitalRead(speedButton) == LOW) {  // ensures button was actually pressed
      fanIndex++;  // increments index, cycling it back to 0 if it is too high
      if (fanIndex > fanSpeedSize - 1) {
        fanIndex = 0;
      }
      analogWrite(powerPin, fanSpeed[fanIndex]);  // changes fan's speed
      delay(200);  //causes a slight delay to ensure button is not accidently pushed "again"
    }
  }
}

void defaultSpeed() {
  if (clock.second >= 30) {  // turns the fan off for 30 sec
    fanIndex = 0;
  }
  else if (clock.second >= 20) {  // turns fan to 1/2 speed for 10 sec
    fanIndex = 1;
  }
  else if (clock.second >= 10) {  // turns fan to 3/4 speed for 10 sec
    fanIndex = 2;
  }
  else {  // turns fan to full speed for 10 sec
    fanIndex = 3;
  }

  analogWrite(powerPin, fanSpeed[fanIndex]);  // determine fan's speed
}

void directionChange() {  // triggered by external interrupt upon button press
    delay(50);
    if (digitalRead(directionButton) == LOW) {  // ensures button was actually pressed
      digitalWrite(directionA, !digitalRead(directionA));  // swaps HIGH/LOW to change direction
      digitalWrite(directionB, !digitalRead(directionB));
      delay(200);  //causes a slight delay to ensure button is not accidently pushed "again"
    }
}

void setDirectionFlag() {  // function that external interrupt directly interfaces with to control the fan's direction
  doChangeDirection = true;  // flag to change fan's direction
  updateLCD = true;  // flag to update LCD display
}

void printTime() {
  // put time into variables
  // prints time to LCD
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
  if (digitalRead(directionA) == HIGH) {  // deterimnes fan's direction off of directionA output
    // prints fan direction to LCD
    lcd.print("CCW   ");
  }
  else {
    lcd.print("CW    ");
  }

  // prints fan's speed to LCD based on the index of the fan
  lcd.setCursor(6,1);
  lcd.print(fanString[fanIndex]);
}
