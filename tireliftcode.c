#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

//Ultra-Sonic Sensor
#define TRIGPIN 5
#define ECHOPIN 18

//Height Button Save
#define SAVEBUTTON 17

//go to distance
#define GOLOCATIONPIN 15

//Relay Pins(Up)
#define UPRELAY 4

//Relay Pins(Down)
#define DOWNRELAY 2

//To control the relays turned on and off on the module
#define OPENRELAY1 27
#define OPENRELAY2 14

//define sound speed in cm/uS
#define SOUND_SPEED 0.034

//byte size to store int 
#define EEPROM_SIZE 1

//mutatable variables
long duration;
int distanceCm = 0;
int saveDistanceCm = 15;

LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C address 0x27, 20 column and 4 rows

void IRAM_ATTR saveDistance(){
  delay(500);
  if(digitalRead(SAVEBUTTON)==LOW){
    saveDistanceCm = distanceCm;
    EEPROM.write(0, saveDistanceCm);
    EEPROM.commit();
  }
}

void IRAM_ATTR moveActuator(){
  bool flag = false;

  delay(500); 
  while(digitalRead(GOLOCATIONPIN) == LOW & digitalRead(SAVEBUTTON) == HIGH){
    while(saveDistanceCm != distanceCm & saveDistanceCm > distanceCm){ //turn on first group of relays
      digitalWrite(UPRELAY, LOW);
      digitalWrite(DOWNRELAY, HIGH);
      digitalWrite(OPENRELAY1, LOW);
      digitalWrite(OPENRELAY2, LOW);

      if(saveDistanceCm == distanceCm){
        flag = true;
        break;
      }
    }

    while(saveDistanceCm != distanceCm & saveDistanceCm < distanceCm){ //turn on second group of relays
      digitalWrite(DOWNRELAY, LOW);
      digitalWrite(UPRELAY, HIGH);
      digitalWrite(OPENRELAY1, LOW);
      digitalWrite(OPENRELAY2, LOW);

      if(saveDistanceCm == distanceCm){
        flag = true;
        break;
      }
    }

    //turns off the relay
    if(flag){
      digitalWrite(UPRELAY, HIGH);
      digitalWrite(DOWNRELAY, HIGH);
      digitalWrite(OPENRELAY1, HIGH);
      digitalWrite(OPENRELAY2, HIGH);
    }
  }
}

void setup() {
  Serial.begin(115200); // Starts the serial communication

  lcd.init(); // initialize the lcd
  lcd.backlight();
  
  //EEPROM to store saved distance
  EEPROM.write(0, saveDistanceCm);
  EEPROM.commit();
  EEPROM.begin(EEPROM_SIZE);
  saveDistanceCm = EEPROM.read(0);
  
  pinMode(TRIGPIN, OUTPUT); // Sets the TRIGPIN as an Output
  pinMode(ECHOPIN, INPUT); // Sets the ECHOPIN as an Input

  //set the relay as output 
  pinMode(UPRELAY, OUTPUT);
  pinMode(DOWNRELAY, OUTPUT);
  pinMode(OPENRELAY1, OUTPUT);
  pinMode(OPENRELAY2, OUTPUT);
  
  pinMode(SAVEBUTTON, INPUT_PULLUP);//Pull up button
  attachInterrupt(digitalPinToInterrupt(SAVEBUTTON), saveDistance, CHANGE); //Save distance interrupt

  pinMode(GOLOCATIONPIN, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(GOLOCATIONPIN), moveActuator, FALLING); //Save distance interrupt
}

void loop() {
  //relays initially off
  digitalWrite(UPRELAY, HIGH);
  digitalWrite(DOWNRELAY, HIGH);
  digitalWrite(OPENRELAY1, HIGH);
  digitalWrite(OPENRELAY2, HIGH);

  // Clears the TRIGPIN
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  // Sets the TRIGPIN on HIGH state for 10 micro seconds
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);
  
  // Reads the ECHOPIN, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHOPIN, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;

  String distanceCmPrint = String(distanceCm);
  String saveddistanceCmPrint = String(saveDistanceCm);

  // Prints the distance in the LCD
  lcd.setCursor(0, 0);            // move cursor the first row
  lcd.print("Distance: " + distanceCmPrint);          // print message at the first row
  lcd.setCursor(0, 1);            // move cursor to the second row
  lcd.print("Saved Distance: " + saveddistanceCmPrint); // print message at the second row

  delay(500);
  lcd.clear();
}
