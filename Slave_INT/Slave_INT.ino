//including all libraries
#include <Multiplex7Seg.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//pin numbers for 7 seg display stored in an array
byte digitPins[] = {9, 10}; // LSB to MSB only using 2 digits
byte segmentPins[] = {2, 3, 4, 5, 6, 7, 8}; // seg a to g

//variables and pin numbers
int temperaturePin = A0;
int PIN = 16;
int SOLE = 17;
int UnoTask_X = -1;   //this value loop in main and does nothing
int selectedTemp;     //variable to hold value of temperature received from galileo
float degreesC = 0;

void setup() 
{
  Multiplex7Seg::set(1, 4, digitPins, segmentPins); // initialize 7 seg
  // Define the LED pin as Output
  pinMode (PIN, OUTPUT);
  pinMode (SOLE, OUTPUT);

  // Start the I2C Bus as Slave on address 8
  Wire.begin(8);
  // Attach a function to trigger when something is received.
  Serial.begin(9600);
  //2 different  functions to communicate over I2C
  //#1 to receive temperature from galileo
  //#2 to send temperature to galileo  
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}
void receiveEvent(int bytes)
{
  UnoTask_X = Wire.read();    // read one character from the I2C
  selectedTemp = Wire.read();
}
void requestEvent() 
{
  Wire.write((byte)degreesC); // respond with message of 1 byte
  // as expected by master
}

void loop()
{
  float voltage;
  voltage = getVoltage(temperaturePin);//analog values between 0-1023
  degreesC = (voltage - 0.5) * 100.0;
  Multiplex7Seg::loadValue(degreesC);//print current temperature to 7 segment
  Serial.println(degreesC);
  delay(300);

  switch (UnoTask_X)
  {
    case 0:
      digitalWrite(SOLE, HIGH);
      break;

    case 1:
      voltage = getVoltage(temperaturePin);
      degreesC = (voltage - 0.5) * 100.0;
      
      Serial.print("  HEATING -> ");
      Serial.println(degreesC);
      delay(200);
      if (degreesC < selectedTemp)
      {
        digitalWrite(PIN, HIGH);
      }
      else
      {
        digitalWrite(PIN, LOW);
        Wire.onRequest(requestEvent);
        UnoTask_X = -1;
      }
      break;

    case 3:
      digitalWrite(SOLE, LOW);    //open door
      UnoTask_X = -1;             //go back to main and do nothing
      break;
  }
}
float getVoltage(int pin)
{
  //converting analog value back to voltage
  //5/1024=0,004882812
  return (analogRead(pin) * 0.004882812);
}
