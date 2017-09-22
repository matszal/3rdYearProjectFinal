
My project idea was to design a model of a washing machine with built in GSM modem.
I chose this project because I thought that the material we have covered in the past two years would allow me to deploy all acquired skills. At the time it was an unique idea on the local market with possible business potential.

Project involves selecting between couple of different operating programs (duty cycles, length of washing, water temperature selection) using a potentiometer. User is able to see selected programs and time remaining on a LCD display. LCD brightness sets automatically using a photodiode. Project consists of a safety door feature, which prevent from accidental activation. Temperature sensor, to control a relay connected to AC sourced heater. LEDs will inform user about heating water status. 2x water pumps to simulate washing chamber in a separate, sealed container.
 At the end of a wash a txt messages are sent to the user, one with acknowledge and second with randomly generated PIN number to open the chamber door.
Hardware components used:
Intel Galileo Gen 2 SoC + Arduino UNO,
DC motor driven by a H-bridge,
LCD screen driven by an I2C chip,
2x Photodiode, to control LCD brightness and simulate insert token/coin function,
2x dc water pumps,
AC heater and a relay,
2x DC water pumps,
Status LEDs,
Program selection potentiometer,
Solenoid, to lock chamber door,
GSM shield.


