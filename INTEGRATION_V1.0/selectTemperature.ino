int selectTemp()
{
  int Temp;
  
  int sensorValue = analogRead(A1);
  int range = map(sensorValue, potMIN, potMAX, 1, 4);
  switch (range)
  {
    case 1:
      Temp = 25;    
      break;
    case 2:
      Temp = 30;   
      break;
    case 3:
      Temp = 35;    
      break;
    case 4:
      Temp = 40;    
      break;
  }
  lcd.setCursor(0, 1);
  lcd.print(Temp);
  return Temp;
}

