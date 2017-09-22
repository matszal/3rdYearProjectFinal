int selectProgram()
{
  int programSelect;  

  int sensorValue1 = analogRead(A1);
  int range1 = map(sensorValue1, potMIN, potMAX, 1, 4);
  switch (range1)
  { //code to select program
    case 1:
      programSelect = 1;  //25:80 cycle
      break;

    case 2:
      programSelect = 2;   //50:50 cycle
      break;

    case 3:
      programSelect = 3;  //80:20 cycle
      break;

    case 4:
      programSelect = 4; //full cycle
      break;

  }
  lcd.setCursor(0, 1);
  lcd.print(range1);
  return programSelect;
}
