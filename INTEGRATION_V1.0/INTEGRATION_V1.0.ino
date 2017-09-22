#include <String.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int UNO = 0x8;  //address of an uno
int UnoTask_X = -1;
int GSM_enable = 7;
int potMIN = 0;
int potMAX = 1000;//due to hardware issue

int PUSH0 = 10;
int en1 = 11;
int en2 = 12;
int photo_pin = 13;
//encoder output pins
int PINA = 8;
int PINB = 6;
int PINC = 5;
int PIND = 4;

int lcd_cursor = 0;
int pin_cursor = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2);
boolean send_txt = 1;
int output, key_code, press_status = 0;
char str[16] = {'1', '2', '3', 'A',
                '4', '5', '6', 'B',
                '7', '8', '9', 'C',
                '*', '0', '#', 'D'
               };

int i = 0;
char num[25] = {"AT+CMGS=\"+353\""};
char temp_num[16];
char temp_pin[5];

//to start finite cycles
int stage = 0;
long randNumber;
int pin;
char ch_pin[5];

int temp123 = 0;
int selectedProgram = 0;;
int selectedTemp;
float voltage, degreesC;

//*********************************************************************SETUP
void setup()
{
  Serial1.begin(9600);
  Serial.begin(9600);
  pinMode(PINA, INPUT);
  pinMode(PINB, INPUT);
  pinMode(PINC, INPUT);
  pinMode(PIND, INPUT);
  pinMode(GSM_enable, OUTPUT);
  pinMode(en1, OUTPUT);
  pinMode(en2, OUTPUT);  
  pinMode(photo_pin, INPUT);
  pinMode(PUSH0, INPUT);
  digitalWrite(GSM_enable, HIGH);
  attachInterrupt(9, Read_Data, FALLING);
  Wire.begin();
  lcd.begin();
  lcd.backlight();
  lcd.print("3rd Year Project");
  delay(500);
  lcd.setCursor(0, 1); //cursor position 0,1 (first row, second column)
  lcd.print("Washing Machine");
  delay(3000);
  lcd.clear();
}

//**********************************************************************LOOP
void loop()
{
  switch (stage)
  {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("Insert token...");
      while (digitalRead(photo_pin) == LOW);
      clearlcd();
      stage++;

    case 1:
      lcd.setCursor(0, 0);
      lcd.print("Enter number:");
      lcd.setCursor(lcd_cursor, 1);
      lcd.blink();
      delay(100);
      if (press_status)
      {
        switch (str[output])//display only numbers on LCD
        {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            temp_num[i] = str[output];
            lcd.print(temp_num[i]);
            lcd.setCursor(lcd_cursor++, 1);
            i++;
            press_status = 0;
            break;

          case 'D': //delete if error
            lcd.setCursor(lcd_cursor--, 1);
            lcd.print(" ");
            temp_num[i] = NULL;
            press_status = 0;
            i--;
            break;

          case '#': //confirm number
            //moving number from temp array  num array:
            //and printing to serial _> later to gsm modem...
            num[13] = temp_num[1];
            num[14] = temp_num[2];
            num[15] = temp_num[3];
            num[16] = temp_num[4];
            num[17] = temp_num[5];
            num[18] = temp_num[6];
            num[19] = temp_num[7];
            num[20] = temp_num[8];
            num[21] = temp_num[9];
            num[22] = '\"';

            clearlcd();
            Serial.println(num);
            lcd_cursor = 0;
            lcd.noBlink();
            press_status = 0;
            i = 0;
            stage++;
            break;
        }
      }
      break;

    case 2:
      lcd.setCursor(0, 0);
      lcd.print("Select Speed");
      selectedProgram = selectProgram();
      if (digitalRead(PUSH0) == HIGH)
      {
        clearlcd();
        delay(50);
        stage++;
      }
      break;

    case 3:
      lcd.setCursor(0, 0);
      lcd.print("Select Temperature");
      selectedTemp = selectTemp();
      if (digitalRead(PUSH0) == HIGH)
      {
        clearlcd();
        delay(50);
        stage++;
      }
      break;
    case 4:
      lcd.setCursor(0, 0);
      lcd.print("temperature/speed");
      lcd.setCursor(0, 1);
      lcd.print(selectedTemp);
      lcd.print("/");
      lcd.print(selectedProgram);
      delay(2000);
      clearlcd();
      lcd.setCursor(0, 0);
      lcd.print("Close door and");
      lcd.setCursor(0, 1);
      lcd.print("press start!");
      
      
      while (digitalRead(PUSH0) == LOW);  //wait for push button
      Wire.beginTransmission(UNO); // transmit to device #8
      Wire.write(UnoTask_X = 0);   // send command to arduino to energize solenoid
      Wire.endTransmission();      // ends a transmission that was begun by begin transmission

      clearlcd();
      stage ++;
      break;

    case 5:
      lcd.setCursor(0, 0);
      lcd.print("Heating");
      Wire.beginTransmission(UNO); // transmit to device #8
      Wire.write(UnoTask_X = 1);   // case 1 at arduino side is to start heating
      Wire.write(selectedTemp);    // sends selected temperature
      Wire.endTransmission();      // ends a transmission

      delay(10);
      temp123 = 0;
      //read temperature value from arduino
      //while temperature is < than selected temperature keep heating
      while (temp123 < selectedTemp)
      {
        Wire.requestFrom(8, 1);
        while (Wire.available())
        {
          temp123 = Wire.read();
          Serial.print("temp");
          Serial.println(temp123);
          delay(100);
        }
      }
      clearlcd();
      stage++;
      break;

    case 6:
      lcd.setCursor(0, 0);
      lcd.print("Now washing...");
      //do dc motor spins 
      for (int i = 0; i < 3; i++)
      {
        goForward();
        Stop1();
        goForward();
        Stop1();
        goBackward();
        Stop1();
        goBackward();
        Stop1();
      }
      clearlcd();
      stage ++;
      break;

    case 7:
      lcd.setCursor(0, 0);
      lcd.print("spin...");
      spin(selectedProgram);
      delay(6000);
      Stop1();
      clearlcd();
      stage++;
      break;

    case 8:
      
      Serial.println(num);
      lcd.setCursor(0, 0);
      lcd.print("GSM...");
      SendTextMessage();
      delay(7000);
      clearlcd();
      stage ++;
      break;

    case 9:      
      lcd.setCursor(0, 0);
      lcd.print("Generating PIN");

      //generate a random number between 1000 and 9999
      randNumber = random(1000, 10000);
      pin = randNumber;

      //converts int type pin value to char value to be ale to send as a text using gsm
      sprintf(ch_pin,"%d",pin);
      SendPIN();
      delay(7000);
      clearlcd();
      stage ++;
      break;

    case 10:
      lcd.setCursor(0, 0);
      lcd.print("Enter PIN");
      lcd.setCursor(pin_cursor, 1);
      lcd.blink();
      delay(100);

      if (press_status)
      {
        switch (str[output])//display only numbers on LCD
        {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            temp_pin[i] = str[output];
            lcd.print("*");
            lcd.setCursor(pin_cursor++, 1);
            i++;
            press_status = 0;
            break;

          case 'D': //delete if error
            lcd.setCursor(pin_cursor--, 1);
            lcd.print(" ");
            temp_pin[i] = NULL;
            press_status = 0;
            i--;
            break;

          case '#': //confirm number

            clearlcd();
            delay(1000);
            pin_cursor = 0;
            lcd.noBlink();
            press_status = 0;
            i = 0;
            clearlcd();
            check_Pin(ch_pin, temp_pin);  //send both pointers to function
            break;
        }
      }
      break;
  }
}

void check_Pin(char x[5], char y[5])
{  
  int a;
  int b;
  //will convert a char type value to an int type to check input pin
  sscanf(x, "%d", &a);  
  sscanf(y, "%d", &b);

  if (a == b)
  {
    lcd.setCursor(0, 0);
    lcd.print("PIN correct :)");
    lcd.setCursor(0,1);
    Wire.beginTransmission(UNO); // transmit to device #8
    Wire.write(UnoTask_X = 3);   // case 3 opens up a door
    Wire.endTransmission();
    lcd.print("Door open!");
    delay(3000);
    clearlcd();
    lcd.setCursor(0, 0);
    lcd.print("Thank you for");
    lcd.setCursor(0,1);
    lcd.print("watching :)");
    delay(2000);
    clearlcd();    
    stage = 0;  //return to insert token
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("Incorrect PIN!");
    lcd.setCursor(0,1);
    lcd.print("try again!!!");
    delay(4000);
    clearlcd();
    stage = 10;      //loop until correct pin entered
  }
}
void clearlcd()
{
  lcd.clear();
  delay(300);
}
void goForward()
{
  digitalWrite(en1, HIGH);
  digitalWrite(en2, LOW);
  delay(300);
}
void goBackward() {
  //left
  digitalWrite(en1, LOW);
  digitalWrite(en2, HIGH);
  delay(700);
}
void Stop1()
{
  digitalWrite(en1, LOW);
  digitalWrite(en2, LOW);
  delay(1500);
}
void spin(int duty_cycle)
{
  //PWM
  int cycle;
  if (duty_cycle == 1)
    cycle = 96;
  if (duty_cycle == 2)
    cycle = 120;
  if (duty_cycle == 3)
    cycle = 200;
  if (duty_cycle == 4)
    cycle = 255;

  analogWrite(en1, cycle);
  digitalWrite(en2, LOW);
}

void SendTextMessage()
{
  Serial1.print("AT+CMGF=1\r"); //Sets the GSM in txt mode
  delay(1000);
  Serial1.println(num);        //array with number
  delay(1000);
  Serial1.println("Wash is finished!!!");
  delay(1000);
  Serial1.println((char)26); //the ASCII code of the ctrl+z is 26 (0x1A)
  delay(1000);
  Serial1.println();

  send_txt = 0;
}

void SendPIN()
{
  Serial1.print("AT+CMGF=1\r"); //Sets the GSM in txt mode
  delay(1000);
  Serial1.println(num);        //array with number
  delay(1000);
  Serial1.println("Your PIN is:");
  Serial1.println(pin);
  delay(1000);
  Serial1.println((char)26); //the ASCII code of the ctrl+z is 26 (0x1A)send message command
  delay(1000);
  Serial1.println();

  send_txt = 0;
}
void Read_Data()
{
  //code to read data from keypad encoder by shifting left data pin
  output = 0;
  output = output | digitalRead(4);
  output = output << 1;
  output = output | digitalRead(5);
  output = output << 1;
  output = output | digitalRead(6);
  output = output << 1;
  output = output | digitalRead(8);
  press_status = 1;
}
float getVoltage(int pin)
{
  return (analogRead(pin) * 0.004882812);
}
