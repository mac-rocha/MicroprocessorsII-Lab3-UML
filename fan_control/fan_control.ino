// libraries needed 
#include <LiquidCrystal.h>
#include <DS3231.h>
#include <Wire.h>

// import clock to maintain accurate timekeeping
DS3231 clock;
RTCDateTime dt; 

// DC motor connections
int ENABLE = 4;
int DIRA = 5;
int DIRB = 6;
int interruptPin = 18;

int button = 3; // button assigned to pin 3

String time = "", speed = "", direction = "";
bool C = false, CC = false;

// initialize "LiquidCrystal" library with numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void setup() {
  // motor control pins setup
  pinMode(ENABLE, OUTPUT);
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);

  pinMode(button, INPUT_PULLUP);

  Serial.begin(9600); // initialize serial communication at 9600 bps
  
  Serial.println("Initialize RTC module");
  clock.begin(); // initialize DS3231
  clock.setDateTime(__DATE__,__TIME__); // sends sketch compiling time to Arduino

  // set up LCD's number of columns and rows
  lcd.begin(16, 2);
  lcd.print("Micro 2");
}

void loop() {
  
  // retrieve time
  getTime();
  
  // check if time is at the beginning of a minute, turn fan on for 30 s 
  if(dt.second == 60000) {
    spinClockwise();
    delay(30000);
  }
}

// function to retrieve info on which direction to spin
void updateInfo() {
  if (C){
    spinCounterClockwise();
  }
  else {
    spinClockwise();
  }
}

// function to spin motor clockwise
void spinClockwise() {
  C = true; 
  CC = false;
  direction = "C"; // set direction string
  
  analogWrite(ENABLE,255); // motor on full speed
  digitalWrite(DIRA,HIGH); // clockwise
  digitalWrite(DIRB,LOW);
  speed = "full";
  getTime();
  setLCD();
  delay(2000);
    
  analogWrite(ENABLE,180); 
  delay(2000);
  
  analogWrite(ENABLE,128); // half speed
  speed = "1/2";
  getTime();
  setLCD();
  delay(2000);
  
  analogWrite(ENABLE,64); // 1/4 speed
  speed = "1/4";
  getTime();
  setLCD();
  delay(2000);
  
  analogWrite(ENABLE,128); //half speed
  speed = "1/2";
  getTime();
  setLCD();
  delay(2000);
  
  analogWrite(ENABLE,192); // 3/4 speed
  speed = "3/4";
  getTime();
  setLCD();
  delay(2000);
  
  analogWrite(ENABLE,255); // full speed
  speed = "Full";
  getTime();
  setLCD();
  delay(2000);
  
  digitalWrite(ENABLE,LOW); // all done
  speed = "0";
  getTime();
  setLCD();
  delay(10000);
}

// function to spin motor counter clockwise
void spinCounterClockwise() {
  C = false; 
  CC = true;
  
  direction = "CC";  // set direction string
  
  analogWrite(ENABLE,255); // motor on full speed
  digitalWrite(DIRA,LOW); // counter clockwise
  digitalWrite(DIRB,HIGH);
  speed = "full";
  getTime();
  setLCD();
  delay(2000);
    
  analogWrite(ENABLE,180); 
  delay(2000);
  
  analogWrite(ENABLE,128); // half speed
  speed = "1/2";
  getTime();
  setLCD();
  delay(2000);
  
  analogWrite(ENABLE,64); // 1/4 speed
  speed = "1/4";
  getTime();
  setLCD();
  delay(2000);
  
  analogWrite(ENABLE,128); //half speed
  speed = "1/2";
  getTime();
  setLCD();
  delay(2000);
  
  analogWrite(ENABLE,192); // 3/4 speed
  speed = "3/4";
  getTime();
  setLCD();
  delay(2000);
  
  analogWrite(ENABLE,255); // full speed
  speed = "Full";
  getTime();
  setLCD();
  delay(2000);
  
  digitalWrite(ENABLE,LOW); //all done
  speed = "0";
  getTime();
  setLCD();
  delay(10000);
}

// function to get time
void getTime() {
  dt=clock.getDateTime(); // get time infromation

  // print time info
  Serial.print("Raw data: ");
  Serial.print(dt.year);   Serial.print("-");
  Serial.print(dt.month);  Serial.print("-");
  Serial.print(dt.day);    Serial.print(" ");
  Serial.print(dt.hour);   Serial.print(":");
  Serial.print(dt.minute); Serial.print(":");
  Serial.print(dt.second); Serial.println("");

  delay(1000);
}

void setLCD()
{
  // set postion
  lcd.setCursor(0,0);

  // print to LCD
  lcd.print(time + " " + speed + " " + direction);
}

// function to check if button has been pressed
bool checkButton() {
  int i;

  // loop to check if button has been pressed
  for (i=0; i<1000; ++i) {
    // if button is pressed
    if (digitalRead(button) == LOW) {
      return false;
    }
    delay(1);
  }
  return true;
}
