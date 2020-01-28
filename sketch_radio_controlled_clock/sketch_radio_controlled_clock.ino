/* Last code change: 2014-04-29 */
#include <Wire.h>                                  //Tells compiler to include Wire (i2c) library
//byte SegValues[] = {B11000000, B11111001, B10100100, B10110000, B10011001, B10010010, B10000011, B11111000, B10000000, B10011000};
byte SegValues[] = {B00111111, B00000110, B01011011, B01001111, B01100110, B01101101, B01111100, B00000111, B01111111, B01100111, B00000000};
//byte SegValues[] = {B00111111, B00000110, B01011011, B01001111, B01100110, B01101101, B01111101, B00000111, B01111111, B01101111, B00000000};
unsigned long difference = 0;
byte units;
byte x10sec2;               //For 100ths of second
byte x10sec1;               //For 10th of seconds
byte second2;               //For seconds units
byte second1;               //For seconds 10s
byte minute2;               //For minute units
byte minute1;               //For minute 10s
byte hour2;                 //For hour units
byte hour1;                 //For hour tens
byte x10sec2lap;            //For 100ths of second paused for lap time
byte x10sec1lap;            //For 10th of seconds paused for lap time
byte second2lap;            //For seconds units paused for lap time
byte second1lap;            //For seconds 10s paused for lap time
byte minute2lap;            //For minute units paused for lap time
byte minute1lap;            //For minute 10s paused for lap time
byte hour2lap;              //For hour units paused for lap time
byte hour1lap;              //For hour tens paused for lap time
byte dig1;                  //Digit 1 - used for stopwatch
byte dig2;                  //Digit 2 - used for stopwatch
byte dig3;                  //Digit 3 - used for stopwatch
byte dig4;                  //Digit 4 - used for stopwatch
byte dig5;                  //Digit 5 - used for stopwatch
byte dig6;                  //Digit 6 - used for stopwatch
byte ledIndicator;          //For setting whether the indicator LED is on or off
boolean started = false;
boolean laptime;
byte thingtoset = 0;        //Variable for deciding which time variable to set in mode 1
byte seconds;
byte minutes;
byte hours;
byte rcminute;
byte rchour;
byte rcyear;
byte rcmonth;
byte rcday;
byte rcdata;
byte mode;
byte segPreference;
unsigned long oldmillis;
boolean buttonstate = true;
byte menubutton = 8;
byte functionbutton = 9;
byte downbutton = 10;
byte upbutton = 11;
int dispDelay = 1000;
byte ghostDelay = 20;
void setup() {
  DDRD = B11111111;                                //Set D0-D7 as outputs
  DDRC = B00001111;                                //Set A0-A3 as outputs
  DDRB = B110000;                                  //Set D8-D11 as inputs and D12-D13 as outputs
  digitalWrite(menubutton, HIGH);                  //Activate internal pullups
  digitalWrite(functionbutton, HIGH);
  digitalWrite(downbutton, HIGH);
  digitalWrite(upbutton, HIGH);
  Wire.begin();
  Wire.beginTransmission(B1101000);
  Wire.write(15);                                  //storage location for character repesentation setting
  Wire.endTransmission();
  Wire.requestFrom(B1101000,1);
  segPreference = Wire.read();
  setSegPreference();
  if (segPreference > 3) {
    Wire.beginTransmission(B1101000);
    Wire.write(15);
    Wire.write(segPreference = 0x00);
    Wire.endTransmission();
  }
}

void setSegPreference() {
  if (segPreference == 0) { SegValues[6] = B01111100; SegValues[7] = B00000111; SegValues[9] = B01100111; } else
  if (segPreference == 1) { SegValues[6] = B01111101; SegValues[7] = B00000111; SegValues[9] = B01100111; } else
  if (segPreference == 2) { SegValues[6] = B01111101; SegValues[7] = B00000111; SegValues[9] = B01101111; } else
  if (segPreference == 3) { SegValues[6] = B01111101; SegValues[7] = B00100111; SegValues[9] = B01101111; }
}

void loop() {
  Wire.beginTransmission(B1101000);
  Wire.write(8);
  Wire.endTransmission();
  Wire.requestFrom(B1101000,1);
  mode = Wire.read();
  if (mode == 1) { goto mode1; }
  if (mode == 2) { goto mode2; }
  if (mode == 3) { goto mode3; }

  mode = 1;                                        //If no mode is stored, mode will be set to 1 however this
                                                   //is deliberately not yet saved to the DS1307's extra memory
  goto mode1;
menu:

  if (digitalRead(menubutton) == HIGH & digitalRead(functionbutton) == HIGH & digitalRead(downbutton) == HIGH & digitalRead(upbutton) == HIGH) {
          buttonstate = false;                               //Re-allow button presses once buttons are released
  }

  if (digitalRead(menubutton) == LOW & buttonstate == false) {
    buttonstate = true;
    if (mode == 1) { goto mode1; }
    if (mode == 2) { goto mode2; }
    if (mode == 3) { goto mode3; }
    if (mode == 4) {
        /* segPreference sets the use of "tails" on the numbers 6, 7 and 9
           0 : No tails
           1 : Tails on 6 only
           2 : Tails on 6 and 9 only
           3 : Tails on 6, 7 and 9 */
      if (segPreference < 3)
        segPreference++;
      else
        segPreference = 0;
      setSegPreference();
      Wire.beginTransmission(B1101000);
      Wire.write(15);
      Wire.write(segPreference);
      Wire.endTransmission();
    }
  }
  if (digitalRead(downbutton) == LOW & buttonstate == false) {
    buttonstate = true;
    mode = mode - 1;
    if (mode == 0) {
      mode = 4; 
    }
  }
  if (digitalRead(upbutton) == LOW & buttonstate == false) {
    buttonstate = true;
    mode = mode + 1;
    if (mode >= 5) { 
      mode = 1; 
    }
  }

  PORTD = SegValues[mode];
  PORTC = B00000001;
  delayMicroseconds(dispDelay);
  PORTC = B00000000;                                //Blank display to prevent ghosting when changing digit
  PORTD = 0;                                        //Blank display to prevent ghosting
  delayMicroseconds(ghostDelay);                             //Allows time for previous transistor to switch off 
                                                    //(will be important when big diaplays are used)
  PORTD = SegValues[10];                            //Change digit
  PORTC = B00000010;                                //Apply new digit
  delayMicroseconds(dispDelay);
  PORTC = B00000000;
  PORTD = 0;
  delayMicroseconds(ghostDelay);
  if (mode == 4)
    PORTD = SegValues[6];
  else
    PORTD = SegValues[10];
  PORTC = B00000100;
  delayMicroseconds(dispDelay);
  PORTC = B00000000;
  PORTD = 0;
  delayMicroseconds(ghostDelay);
  if (mode == 4)
    PORTD = SegValues[7];
  else
    PORTD = SegValues[10];
  PORTC = B00001000;
  delayMicroseconds(dispDelay);
  PORTC = B00000000;
  PORTD = 0;
  delayMicroseconds(ghostDelay);
  if (mode == 4)
    PORTD = SegValues[9];
  else
    PORTD = SegValues[10];
  PORTB = B00101111;
  delayMicroseconds(dispDelay);
  PORTB = B00001111;
  PORTD = 0;
  delayMicroseconds(ghostDelay);
  PORTD = SegValues[10];
  PORTB = B00011111;
  delayMicroseconds(dispDelay);
  PORTB = B00001111;
  PORTD = 0;
  delayMicroseconds(ghostDelay);
  goto menu;
mode1:                                             //************************** MODE 1 - CLOCK MODE ***************************
  Wire.beginTransmission(B1101000);                //Initializes i2c transmission to DS1307
  Wire.write(8);                                   //Sets address pointer to 8
  Wire.write(1);                                   //Writes value of 1 to this address to remember the last mode on power-off
  Wire.endTransmission();
  Wire.beginTransmission(B1011000);                //Initializes i2c transmission to PICAXE
  Wire.write(64);                                  //Sets address pointer to 64
  Wire.write((uint8_t) 0);                         //Writes 0 - this is to clear the radio controlled clock signal parity pass
                                                    //bit so that old data that was received whilst using another mode does not
                                                    //overwrite the ime on the DS1307 or set the seconds
  Wire.endTransmission();
  oldmillis = 0;                                   //Clears oldmillis variable
  ledIndicator = 0;                                //Ensures indicator LED is off
mode1loop:    

  if (digitalRead(menubutton) == HIGH & digitalRead(functionbutton) == HIGH & digitalRead(downbutton) == HIGH & digitalRead(upbutton) == HIGH) {
    buttonstate = false;                               //Re-allow button presses once buttons are released
  }

  if ((oldmillis + 300000) <= millis()) {          //Turns off indicator LED if 5 minutes (300 seconds) have passed
    ledIndicator = 0x00;                            //since the last time synchronisation
  }
  Wire.beginTransmission(B1011000);                //Initializes i2c transmission to PICAXE
  Wire.write(64);                                  //Sets address pointer to 64 (location 64 on PICAXE scratchpad)
  Wire.endTransmission();
  Wire.requestFrom(B1011000,6);
  rcdata = Wire.read();                            //Reads variable that specifies whether the PICAXE has received new time or not
  if (rcdata == 1) {
    rcyear = Wire.read();                          //Reads year byte from PICAXE
    rcmonth = Wire.read();                         //Reads month byte from PICAXE
    rcday = Wire.read();                           //Reads day byte from PICAXE
    rchour = Wire.read();                          //Reads hour byte from PICAXE
    rcminute = Wire.read();                        //Reads minute byte from PICAXE
    Wire.beginTransmission(B1101000);              //Initializes i2c transmission to DS1307
    Wire.write((uint8_t) 0);                       //Sets the address pointer in the DS1307 to 0
    Wire.write((uint8_t) 0);                       //Writes 0 to seconds register
    Wire.write(rcminute);                          //Writes newly received minute to minutes register
    Wire.write(rchour);                            //Writes newly received hour to hour register
    Wire.write(rcday);                             //Writes newly received day to day register (not day of week)
    Wire.write(rcmonth);                           //Writes newly received month to month register
    Wire.write(rcyear);                            //Writes newly received year to year register
    Wire.endTransmission();
    Wire.beginTransmission(B1011000);              //Initializes i2c transmission to PICAXE
    Wire.write(64);                                //Sets address pointer in the PICAXE to 64
    Wire.write((uint8_t) 0);                       //Writes 0 so that the data isn't read again
    Wire.endTransmission();
    ledIndicator = 0x80;                           //Turns on indicator LED
    oldmillis = millis();
  }

  Wire.beginTransmission(B1101000);                //Initializes i2c transmission to DS1307
  Wire.write((uint8_t) 0);                         //Sets the address pointer in the DS1307 to 0 (seconds address)
  Wire.endTransmission();
  Wire.requestFrom(B1101000, 3);                   //Reads three bytes (seconds, minutes, hours) from DS1307
  seconds = Wire.read();
  minutes = Wire.read();
  hours = Wire.read();

  if (digitalRead(upbutton) == LOW & buttonstate == false)  //Find out if up button is pressed
  {
    if (thingtoset == 1) {                        //Hours set mode
      Wire.beginTransmission(B1101000);            //Initializes i2c transmission to DS1307
      Wire.write(2);                               //Sets the address pointer in the DS1307 to 2 (hours address)
  
      if (hours < 0x09 | (hours < 0x19 & hours > 0x0F) | (hours > 0x1F & hours < 0x23))
      {                                            //Increments hours normally
        Wire.write(hours + 1);                     //for hours values <> 9, <> 19, < 23
      }                                            //
      else if (hours == 0x09 | hours == 0x19)      //Increments hours abnormally
      {                                            //for hours values 9, 19 because
        Wire.write(hours + 7);                     //DS1307 stores time in BCD not binary
      }                                            //so, for example, hour 19 = 0x19 and hour 20 = 0x20,
      else                                         //missing out 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
      {                                            //
        Wire.write((uint8_t) 0);                   //Resets hours to 0 because this line only runs if
      }                                            //hours >= 23 so hours needs to be reset
      Wire.endTransmission();                      //Ends transmission to DS1307
    }
    if (thingtoset == 2) {                        //Minute set mode
      Wire.beginTransmission(B1101000);            //Initializes i2c transmission to DS1307
      Wire.write(1);                               //Sets address pointer in DS1307 to 1 (minutes address)
  
      if (((minutes & B00001111) < 9) & minutes < 0x59)
      {
        Wire.write(minutes + 1);
      }
      else if ((minutes & B00001111) == 9 & minutes < 0x59)
      {
        Wire.write(minutes + 7);
      }
      else
      {
        Wire.write((uint8_t) 0);
      }
      Wire.endTransmission();
    }
    if (thingtoset == 3) {                        //Second clear mode
      Wire.beginTransmission(B1101000);            //Initializes i2c transmission to DS1307
      Wire.write((uint8_t) 0);                     //Sets address pointer in DS1307 to 0 (seconds address)
      Wire.write((uint8_t) 0);                     //Sets seconds to 0
      Wire.endTransmission();
    }
    buttonstate = true;                           //buttonState variable set to true to stop this block of code
                                                   //repeating rapidly if the button is held down
  }

  if (digitalRead(downbutton) == LOW & buttonstate == false)  //Find out if down button is pressed
  {
    if (thingtoset == 1) {                        //Hour set mode
      Wire.beginTransmission(B1101000);            //Initializes i2c transmission to DS1307
      Wire.write(2);                               //Sets the address pointer in the DS1307 to 2 (hours address)
  
      if ((hours > 0 & hours < 0x10) | (hours < 0x20 & hours > 0x10) | (hours > 0x20 & hours < 0x24))
      {                                            //Decrements hours normally
        Wire.write(hours - 1);                     //for hours values <> 9, <> 19, < 23
      }
      else if (hours == 0) {
        Wire.write(0x23);                          //Sets hours to 23 if hours is currently set to 0
      }
      else if (hours == 0x10 | hours == 0x20)      //Decrements hours abnormally
      {                                            //for hours values 9, 19 because
        Wire.write(hours - 7);                     //DS1307 stores time in BCD not binary
      }                                            //so, for example, hour 19 = 0x19 and hour 20 = 0x20,
      else                                         //missing out 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
      {
        Wire.write((uint8_t) 0);                   //Resets hours to 0 because this line only runs if
      }                                            //hours >= 23 so hours needs to be reset
      Wire.endTransmission();                      //Ends transmission to DS1307
    }
    if (thingtoset == 2) {                        //Minute set mode
      Wire.beginTransmission(B1101000);            //Initializes i2c transmission to DS1307
      Wire.write(1);                               //Sets pointer address in DS1307 to 1 (minutes address)
  
      if (((minutes & B00001111) > 0) & minutes < 0x60)
      {
        Wire.write(minutes - 1);
      }
      else if (minutes == 0) {
        Wire.write(0x59);
      }
      else if ((minutes & B00001111) == 0 & minutes < 0x60)
      {
        Wire.write(minutes - 7);
      }
      else
      {
        Wire.write((uint8_t) 0);
      }
      Wire.endTransmission();
    }
    if (thingtoset == 3) {                        //Second clear mode
      Wire.beginTransmission(B1101000);            //Initializes i2c transmission to DS1307
      Wire.write((uint8_t) 0);                     //Sets pointer address in DS1307 to 0 (seconds address)
      Wire.write((uint8_t) 0);                     //Sets seconds to 0
      Wire.endTransmission();
    }
    buttonstate = true;                            //buttonState variable set to true to stop this block of code
  }                                                 //rapidly repeating if the button is held down

  if (digitalRead(functionbutton) == LOW & buttonstate == false)  //Find out if functionbutton is pressed
  {
    buttonstate = true;                            //buttonState variable set to true to stop this block of code
    if (thingtoset < 3) {                           //rapidly repeating if the button is held down
      thingtoset = thingtoset + 1;                 //thingtoset is incremented so that the next value (hour/minute/second)
    } else {                                        //can be changed with the up/down buttons
      thingtoset = 0;
    }
  }

  if (digitalRead(menubutton) == LOW & digitalRead(upbutton) == HIGH & digitalRead(downbutton) == HIGH & digitalRead(functionbutton) == HIGH & buttonstate == false) {
    buttonstate = true;
    thingtoset = 0;                                //Set thingtoset to 0
    goto menu;                                     //Jump to menu label (goes to menu)
  }

  if (thingtoset == 1) {                           //Finds out if clock is in hour set mode (code below executed if it is)
    if ((hours >> 4) == 0 | (millis() & 0x80) == 0x80) {
      PORTD = 0;                                   //Makes first digit blank if it is 0 (leading zero blanking)
    } else {
      PORTD = SegValues[(hours >> 4)];             //Makes PORTD equal to segment pattern from SegValues array
    }                                               //chosen by upper nybble of hours byte
    PORTC = B00000001;
    delayMicroseconds(dispDelay);
    PORTC = B00000000;                             //Blank display to prevent ghosting when changing digit
    PORTD = 0;                                      //Blank display to prevent ghosting
    delayMicroseconds(ghostDelay);                 //Allows time for previous transistor to switch off 
                                                    //(will be important when big displays are used)
    if ((millis() & 0x80) == 0x80) {               //Checks whether bit 3 of millis is 1 or not
      PORTD = B10000000;                           //Blanks display (except for colon) if bit 3 of millis was 1
    } else {                                        //(makes digit flash due to state of bit 3 changing a few times a second)
      PORTD = SegValues[(hours & B1111)] | B10000000;   //Makes PORTD equal to segment pattern from SegValues array
    }                                                    //chosen by lower nybble of hours byte
  } else {                                         //Code below executed if not in hour set mode
    if ((hours >> 4) == 0) {
      PORTD = 0;                                   //Makes first digit blank if it is 0 (leading zero blanking)
    } else {
      PORTD = SegValues[(hours >> 4)];
    }
    PORTC = B00000001;
    delayMicroseconds(dispDelay);
    PORTC = B00000000;                             //Blank display to prevent ghosting when changing digit
    PORTD = 0;                                     //Blank display to prevent ghosting
    delayMicroseconds(ghostDelay);                 //Allows time for previous transistor to switch off 
                                                    //(will be important when big diaplays are used)
    PORTD = SegValues[(hours & B1111)] | B10000000;   //Change digit
  }
  PORTC = B00000010;                                //Apply new digit
  delayMicroseconds(dispDelay);
  PORTC = B00000000;
  PORTD = 0;
  delayMicroseconds(ghostDelay);

  if (thingtoset == 2 & (millis() & 0x80) == 0x80) {
    PORTD = 0;
  } else {
    PORTD = SegValues[(minutes >> 4)];
  }
  PORTC = B00000100;
  delayMicroseconds(dispDelay);
  PORTC = B00000000;
  PORTD = 0;
  delayMicroseconds(ghostDelay);
  if (thingtoset == 2 & (millis() & 0x80) == 0x80) {
    PORTD = B10000000;
  } else {
    PORTD = SegValues[(minutes & B1111)] | B10000000;
  }
    
  PORTC = B00001000;
  delayMicroseconds(dispDelay);
  PORTC = B00000000;
  PORTD = 0;
  delayMicroseconds(ghostDelay);
  if (thingtoset == 3 & (millis() & 0x80) == 0x80) {
    PORTD = 0;
  } else {
    PORTD = SegValues[(seconds >> 4)];
  }
  PORTB = B00101111;
  delayMicroseconds(dispDelay);
  PORTB = B00001111;
  PORTD = 0;
  delayMicroseconds(ghostDelay);
  if (thingtoset == 3 & (millis() & 0x80) == 0x80) {
    PORTD = 0;
  } else {
    PORTD = SegValues[(seconds & B1111)] | ledIndicator;
  }
  PORTB = B00011111;
  delayMicroseconds(dispDelay);
  PORTB = B00001111;
  PORTD = 0;
//  delayMicroseconds(ghostDelay);
  goto mode1loop;


mode2:                                             //********************* MODE 2 - COUNTDOWN MODE ****************************
  Wire.beginTransmission(B1101000);                //Initializes i2c transmission to DS1307
  Wire.write(8);                                   //Sets address pointer to 8
  Wire.write(2);                                   //Writes value of 2 to this address to remember the last mode on power-off
  Wire.endTransmission();
  Wire.requestFrom(B1101000,6);
  second2 = Wire.read();
  second1 = Wire.read();
  minute2 = Wire.read();
  minute1 = Wire.read();
  hour2 = Wire.read();
  hour1 = Wire.read();
  if (second2 > 9 | second1 > 5 | minute2 > 9 | minute1 > 5 | hour2 > 9 | hour1 > 9) {      //Deletes saved numbers
    second2 = 0;                                                                            //if any are greater than
    second1 = 0;                                                                            //9 (this would indicate
    minute2 = 0;                                                                            //that the data is
    minute1 = 0;                                                                            //corrupted)
    hour2 = 0;
    hour1 = 0;
  }
mode2loop:
  if (digitalRead(menubutton) == LOW & digitalRead(upbutton) == HIGH & digitalRead(downbutton) == HIGH & digitalRead(functionbutton) == HIGH & started == false & buttonstate == false) { 
    buttonstate = true;
    thingtoset = 0;
    goto menu; 
  }                           //Check menu button

  if (digitalRead(functionbutton) == LOW & digitalRead(upbutton) == HIGH & digitalRead(downbutton) == HIGH & buttonstate == false) {  //Check function button
      buttonstate = true;                                                        //Makes buttonstate true so that repeated actions don't occur
      if (thingtoset < 4) {
        thingtoset = thingtoset + 1;
      } else {
        thingtoset = 0;
      }
      if (thingtoset == 4) {
        Wire.beginTransmission(B1101000);          //Initializes i2c transmission to DS1307
        Wire.write(9);                             //Sets address pointer to 9
        Wire.write(second2);
        Wire.write(second1);
        Wire.write(minute2);
        Wire.write(minute1);
        Wire.write(hour2);
        Wire.write(hour1);
        Wire.endTransmission();
      }
  }
  
  
  if (digitalRead(downbutton) == LOW & thingtoset != 4 & buttonstate == false) {
    if (thingtoset == 1) {
      hour2 = hour2 + 1;
    } else if (thingtoset == 2) {
      minute2 = minute2 + 1;
    } else if (thingtoset == 3) {
      second2 = second2 + 1;
    }
    buttonstate = 1;
  }
  if (digitalRead(upbutton) == LOW & thingtoset != 4 & buttonstate == false)  {
    if (thingtoset == 1) {
      hour2 = hour2 - 1;
    } else if (thingtoset == 2) {
      minute2 = minute2 - 1;
    } else if (thingtoset == 3) {
      second2 = second2 - 1;
    }
    buttonstate = 1;
  }

  if (digitalRead(menubutton) == HIGH & digitalRead(functionbutton) == HIGH & digitalRead(downbutton) == HIGH & digitalRead(upbutton) == HIGH) {
          buttonstate = false;                               //Re-allow button presses once buttons are released
  }   

  units = millis()/100 - difference;
  if (thingtoset == 4)  {
    if (units >= 10)  {
      difference = difference + 10;
      second2 = second2 + 1;  
    }
  }
  else { 
    if (units >= 10)  {
      difference = difference + 10; 
    }
  }

  if (second2 == 255)  {
    second1 = second1 - 1;
    second2 = 9;  
  }
  if (second2 == 10)  {
    second1 = second1 + 1;
    second2 = 0;  
  }
  if (second1 == 255) {
    if (thingtoset == 4)  {
      minute2 = minute2 - 1;
    }
    second1 = 5;  
  }
  if (second1 == 6) {
    if (thingtoset == 4) {
      minute2 = minute2 + 1;
    }
    second1 = 0;  
  }
  if (minute2 == 255)  {
    minute1 = minute1 - 1;
    minute2 = 9;  
  }
  if (minute2 == 10)  {
    minute1 = minute1 + 1;
    minute2 = 0;  
  }
  if (minute1 == 255) {
    if (thingtoset == 4)  {
      hour2 = hour2 - 1;
    }
    minute1 = 5;  
  }
  if (minute1 == 6) {
    if (thingtoset == 4) {
      hour2 = hour2 + 1;
    }
    minute1 = 0;  
  }
  if (hour2 == 255) {
    hour1 = hour1 - 1;
    hour2 = 9;  
  }
  if (hour2 == 10) {
    hour1 = hour1 + 1;
    hour2 = 0;  
  }
  if (hour1 == 255) {
    hour1 = 9;
  }
  if (hour1 > 9) {
    hour1 = 0;
  }
  if (hour1 == 9 & hour2 == 9 & minute1 == 5 & minute2 == 9 & second1 == 5 & second2 == 9)  {
    if (thingtoset != 4)  {
      hour1 = 9;
      hour2 = 9;
      minute1 = 5;
      minute2 = 9;
      second1 = 5;
      second2 = 9;  
    }
    else  {
      thingtoset = 0;
    }
  }
  
  if (((9 - hour1) == 0) | (thingtoset == 1 & (millis() & 0x80) == 0x80)) {
    PORTD = 0;
  } else {
    PORTD = SegValues[(9 - hour1)];
  }
  PORTC = B00000001;
  delayMicroseconds(dispDelay);
  PORTC = B00000000;                               //Blank display to prevent ghosting when changing digit
  PORTD = 0;
  delayMicroseconds(ghostDelay);                            //Allows time for previous transostor to switch off
  if (thingtoset == 1 & (millis() & 0x80) == 0x80) {
    PORTD = B10000000;
  } else {
    PORTD = SegValues[(9 - hour2)] | B10000000;  //Change digit
  }
  PORTC = B00000010;                               //Apply new digit
  delayMicroseconds(dispDelay);
  PORTC = B00000000;
  PORTD = 0;
  delayMicroseconds(ghostDelay);                            //Allows time for previous transistor to switch off
  if (thingtoset == 2 & (millis() & 0x80) == 0x80) {
    PORTD = 0;
  } else {
    PORTD = SegValues[(5 - minute1)];
  }
  PORTC = B00000100;
  delayMicroseconds(dispDelay);
  PORTC = B00000000;
  PORTD = 0;
  delayMicroseconds(ghostDelay);                            //Allows time for previous transistor to switch off
  if (thingtoset == 2 & (millis() & 0x80) == 0x80) {
    PORTD = B10000000;
  } else {
    PORTD = SegValues[(9 - minute2)] | B10000000;
  }
  PORTC = B00001000;
  delayMicroseconds(dispDelay);
  PORTC = B00000000;
  PORTD = 0;
  delayMicroseconds(ghostDelay);                            //Allows time for previous transistor to switch off
  if (thingtoset == 3 & (millis() & 0x80) == 0x80) {
    PORTD = 0;
  } else {
    PORTD = SegValues[(5 - second1)];
  }
  PORTB = B00101111;
  delayMicroseconds(dispDelay);
  PORTB = B00001111;
  PORTD = 0;
  delayMicroseconds(ghostDelay);                            //Allows time for previous transistor to switch off
  if (thingtoset == 3 & (millis() & 0x80) == 0x80) {
    PORTD = 0;
  } else {
    PORTD = SegValues[(9 - second2)];
  }
  PORTB = B00011111;
  delayMicroseconds(dispDelay);
  PORTB = B00001111;
  PORTD = 0;
//  delayMicroseconds(ghostDelay);
  goto mode2loop;


mode3:
                                           //*************************** MODE 3 - STOPWATCH ***************************
  Wire.beginTransmission(B1101000);                //Initializes i2c transmission to DS1307
  Wire.write(8);                                   //Sets address pointer to 8
  Wire.write(3);                                   //Writes value of 3 to this address to remember the last mode on power-off
  Wire.endTransmission();
  hour1 = 0;
  hour2 = 0;
  minute1 = 0;
  minute2 = 0;
  second1 = 0;
  second2 = 0;
  x10sec1 = 0;
  x10sec2 = 0;
  rcdata = 1;                  //Default display mode is mm:ss:1/10s - rcdata variable is reused for this
  laptime = false;
mode3loop:
  if (digitalRead(menubutton) == LOW & digitalRead(upbutton) == HIGH & digitalRead(downbutton) == HIGH & digitalRead(functionbutton) == HIGH & started == false & buttonstate == false) { 
    buttonstate = true;
    goto menu;
  }                           //Check menu button

  if (digitalRead(functionbutton) == LOW & digitalRead(upbutton) == HIGH & digitalRead(downbutton) == HIGH) { 
    if (buttonstate == false) {                          //Check function button
      buttonstate = true;                                //Makes buttonstate true so that repeated actions don't occur
      if (started == true) { 
        started = false;                                 //Toggle whether the timer is running
        laptime = false;
      } 
      else { 
        started = true;
      }
    }
  }
  
  if (digitalRead(upbutton) == LOW & buttonstate == false) {
    buttonstate = true;
    if (started == true) {
      if (laptime == 0) {
        laptime = 1;
        hour1lap = hour1;
        hour2lap = hour2;
        minute1lap = minute1;
        minute2lap = minute2;
        second1lap = second1;
        second2lap = second2;
        x10sec1lap = x10sec1;
        x10sec2lap = x10sec2;
      } else {
        laptime = 0;
      }
    } else {
      hour1 = 0;
      hour2 = 0;
      minute1 = 0;
      minute2 = 0;
      second1 = 0;
      second2 = 0;
      x10sec1 = 0;
      x10sec2 = 0;
    }
  }
  
  if (digitalRead(downbutton) == LOW & buttonstate == false) {
    buttonstate = true;
    if (rcdata == 0) {
      rcdata = 1;
    } else { if (rcdata == 1) {
      rcdata = 2;
    } else {
      rcdata = 0;
    }}
  }


  if (digitalRead(menubutton) == HIGH & digitalRead(functionbutton) == HIGH & digitalRead(downbutton) == HIGH & digitalRead(upbutton) == HIGH) {
          buttonstate = false;                               //Re-allow button presses once buttons are released
  }

  units = millis() - difference;
  if (started == true)  {
    if (units >= 10)  {
      difference = difference + 10;
      x10sec2 = x10sec2 + 1;  
    }
  }  else { 
    if (units >= 10)  {
      difference = difference + 10; 
    }
  }
  if (x10sec2 > 9) {
    x10sec1 = x10sec1 + 1;
    x10sec2 = 0;
  }
  if (x10sec1 > 9) {
    second2 = second2 + 1;
    x10sec1 = 0;
  }
  if (second2 > 9)  {
    second1 = second1 + 1;
    second2 = 0;  
  }
  if (second1 > 5) {
    minute2 = minute2 + 1;
    second1 = 0;  
  }
  if (minute2 > 9)  {
    minute1 = minute1 + 1;
    minute2 = 0;  
  }
  if (minute1 > 5) {
    hour2 = hour2 + 1;
    minute1 = 0;  
  }
  if (hour2 > 9) {
    hour1 = hour1 + 1;
    hour2 = 0;  
  }
  if (hour1 > 9) {
    hour1 = 0;
  }
  if (hour1 == 9 & hour2 == 9 & minute1 == 5 & minute2 == 9 & second1 == 5 & second2 == 9)  {
      hour1 = 0;
      hour2 = 0;
      minute1 = 0;
      minute2 = 0;
      second1 = 0;
      second2 = 0;
  }
  if (laptime == false) {
    if (rcdata == 1) {
      dig1 = SegValues[minute1];                     //No leading zero blanking used here
      dig2 = SegValues[minute2] | B10000000;         //because the first displayed value isn't the
      dig3 = SegValues[second1];                     //first value and using blanking here could falsely
      dig4 = SegValues[second2] | B10000000;         //suggest a lack of the hours value
      dig5 = SegValues[x10sec1];
      dig6 = SegValues[x10sec2];
    } else if (rcdata == 2) {
      dig1 = SegValues[hour2] | B10000000;           //No leading zero blanking used here
      dig2 = SegValues[minute1];                     //because first value displayed is single digit
      dig3 = SegValues[minute2] | B10000000;
      dig4 = SegValues[second1];
      dig5 = SegValues[second2] | B10000000;
      dig6 = SegValues[x10sec1];
    } else {
      if (hour1 == 0) {                              //Leading zero blanking is used here
        dig1 = 0;
      } else {
        dig1 = SegValues[hour1];
      }
      dig2 = SegValues[hour2] | B10000000;
      dig3 = SegValues[minute1];
      dig4 = SegValues[minute2] | B10000000;
      dig5 = SegValues[second1];
      dig6 = SegValues[second2];
    }
  } else {
    if (rcdata == 1) {
      dig1 = SegValues[minute1lap];
      dig2 = SegValues[minute2lap] | B10000000;
      dig3 = SegValues[second1lap];
      dig4 = SegValues[second2lap] | B10000000;
      dig5 = SegValues[x10sec1lap];
      dig6 = SegValues[x10sec2lap];
    } else if (rcdata == 2) {
      dig1 = SegValues[hour2lap] | B10000000;
      dig2 = SegValues[minute1lap];
      dig3 = SegValues[minute2lap] | B10000000;
      dig4 = SegValues[second1lap];
      dig5 = SegValues[second2lap] | B10000000;
      dig6 = SegValues[x10sec1lap];
    } else {
      if (hour1lap == 0) {
        dig1 = 0;
      } else {
        dig1 = SegValues[hour1lap];
      }
      dig2 = SegValues[hour2lap] | B10000000;
      dig3 = SegValues[minute1lap];
      dig4 = SegValues[minute2lap] | B10000000;
      dig5 = SegValues[second1lap];
      dig6 = SegValues[second2lap];
    }
  }
  
  PORTD = dig1;
  PORTC = B00000001;
  delayMicroseconds(dispDelay);
  PORTC = B00000000;                                        //Blank display to prevent ghosting when changing digit
  PORTD = 0;
  delayMicroseconds(ghostDelay);                            //Allows time for previous transostor to switch off
  PORTD = dig2;                                             //Change digit
  PORTC = B00000010;                                        //Apply new digit
  delayMicroseconds(dispDelay);
  PORTC = B00000000;
  PORTD = 0;
  delayMicroseconds(ghostDelay);                            //Allows time for previous transistor to switch off
  PORTD = dig3;
  PORTC = B00000100;
  delayMicroseconds(dispDelay);
  PORTC = B00000000;
  PORTD = 0;
  delayMicroseconds(ghostDelay);                            //Allows time for previous transistor to switch off
  PORTD = dig4;
  PORTC = B00001000;
  delayMicroseconds(dispDelay);
  PORTC = B00000000;
  PORTD = 0;
  delayMicroseconds(ghostDelay);                            //Allows time for previous transistor to switch off
  PORTD = dig5;
  PORTB = B00101111;
  delayMicroseconds(dispDelay);
  PORTB = B00001111;
  PORTD = 0;
  delayMicroseconds(ghostDelay);                            //Allows time for previous transistor to switch off
  PORTD = dig6;
  PORTB = B00011111;
  delayMicroseconds(dispDelay);
  PORTB = B00001111;
  PORTD = 0;
//  delayMicroseconds(ghostDelay);
  goto mode3loop;
}
