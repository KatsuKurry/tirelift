#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

//Ultra-Sonic Sensor
#define trigPin 5
#define echoPin 18

//Height Button Save
#define buttonPin 17

//
#define golocationPin 15

//Relay Pins(Up)
#define upRelay 4

//Relay Pins(Down)
#define downRelay 2

//Yes and No Buttons
#define YES 12
#define NO 13

//define sound speed in cm/uS
#define SOUND_SPEED 0.034

//byte size to store int 
#define EEPROM_SIZE 1

long duration;
int distanceCm;
int saveddistanceCM;

String distanceCmPrint;
String saveddistanceCmPrint;

LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C address 0x27, 20 column and 4 rows

void IRAM_ATTR saveDistance(){
  saveddistanceCM = distanceCm;
  EEPROM.write(0, saveddistanceCM);
  EEPROM.commit();
}

void IRAM_ATTR moveActuator(){
  delay(200); //needs to be changed to millis
  while(saveddistanceCM > distanceCm){ //turn on first group of relays
    digitalWrite(upRelay, HIGH);
  }

  while(saveddistanceCM < distanceCm){ //turn on second group of relays
    digitalWrite(downRelay, HIGH);
  }

}

void IRAM_ATTR reset(){
  while(saveddistanceCM < distanceCm && saveddistanceCM != distanceCm){ //turn on second group of relays
    digitalWrite(downRelay, HIGH);
  }
  
  digitalWrite(downRelay, LOW);
}

void setup() {
  lcd.init(); // initialize the lcd
  lcd.backlight();
  
  Serial.begin(115200); // Starts the serial communication
  EEPROM.begin(EEPROM_SIZE);
  saveddistanceCM = EEPROM.read(0);
  
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(upRelay, OUTPUT);
  pinMode(downRelay, OUTPUT);
  
  pinMode(buttonPin, INPUT_PULLUP);//Pull up button
  attachInterrupt(digitalPinToInterrupt(buttonPin), saveDistance, CHANGE); //Save distance interrupt

  pinMode(golocationPin, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(golocationPin), moveActuator, CHANGE); //Save distance interrupt
}

void loop() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;

  distanceCmPrint = String(distanceCm);
  saveddistanceCmPrint = String(saveddistanceCM);

  // Prints the distance in the LCD
  lcd.setCursor(0, 0);            // move cursor the first row
  lcd.print("Distance: " + distanceCmPrint);          // print message at the first row
  lcd.setCursor(0, 1);            // move cursor to the second row
  lcd.print("Saved Distance: " + saveddistanceCmPrint); // print message at the second row

  //stops the actuator from moving
  if(saveddistanceCM != distanceCm || saveddistanceCM > distanceCm){
    digitalWrite(upRelay, LOW);
  }
  if(saveddistanceCM != distanceCm || saveddistanceCM < distanceCm){
    digitalWrite(downRelay, LOW);
  }

  delay(500);
  lcd.clear();
}
