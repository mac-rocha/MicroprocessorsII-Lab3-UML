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

int button = 3; // button assigned to pin 3

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
}

void loop() {
  
  // retrieve time
  getTime();
  
  // check if time is at the beginning of a minute, turn fan on for 30 s 
  if(dt.second == 60000) {
    spinMotor();
    delay(30000);
  }

  // display current time
  lcd.setCursor(0,1);
  lcd.print(dt.second/1000); // divide by 1000 because time is retrieved in milliseconds

  // display clockwise direction and speed on LCD if motor is spinning clockwise
  if(DIRA==HIGH) {
    if(ENABLE==255) {
      lcd.print("C Full");
    }
    else if(ENABLE==128) {
      lcd.print("C 1/2");
    }
    else if(ENABLE==64) {
      lcd.print("C 1/4");
    }
    else if(ENABLE==192) {
      lcd.print("C 3/4");
    }
    else if(ENABLE==LOW) {
      lcd.print("C 0");
    }
  }

  // display counter clockwise direction and speed on LCD if motor is spinning CC
  else if(DIRA==LOW) {
    if(ENABLE==255) {
      lcd.print("CC Full");
    }
    else if(ENABLE==128) {
      lcd.print("CC 1/2");
    }
    else if(ENABLE==64) {
      lcd.print("CC 1/4");
    }
    else if(ENABLE==192) {
      lcd.print("CC 3/4");
    }
    else if(ENABLE==LOW) {
      lcd.print("CC 0");
    }
  }
}

// function to spin motor  
void spinMotor() {
  analogWrite(ENABLE,255); // motor on full speed
  digitalWrite(DIRA,HIGH); // clockwise
  digitalWrite(DIRB,LOW);
  delay(2000);
  analogWrite(ENABLE,180); 
  delay(2000);
  analogWrite(ENABLE,128); // half speed
  delay(2000);
  analogWrite(ENABLE,64); // 1/4 speed
  delay(2000);
  analogWrite(ENABLE,128); //half speed
  delay(2000);
  analogWrite(ENABLE,192); // 3/4 speed
  delay(2000);
  analogWrite(ENABLE,255); // full speed
  delay(2000);
  digitalWrite(ENABLE,LOW); //all done
  delay(10000);

  analogWrite(ENABLE,255); // motor on full speed
  digitalWrite(DIRA,LOW); // counter clockwise
  digitalWrite(DIRB,HIGH);
  delay(2000);
  analogWrite(ENABLE,180); 
  delay(2000);
  analogWrite(ENABLE,128); // half speed
  delay(2000);
  analogWrite(ENABLE,64); // 1/4 speed
  delay(2000);
  analogWrite(ENABLE,128); //half speed
  delay(2000);
  analogWrite(ENABLE,180); 
  delay(2000); 
  analogWrite(ENABLE,255); // full speed
  delay(2000);
  digitalWrite(ENABLE,LOW); //all done
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
