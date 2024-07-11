/*

  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 modified 7 Nov 2016
 by Arturo Guadalupi

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystalHelloWorld

*/

/*
July 10, 2024, rebuild the new world
Step 1: Installing ESP8266 Board in Arduino IDE
Enter http://arduino.esp8266.com/stable/package_esp8266com_index.json into the “Additional Boards Manager URLs” field
Ref: https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/

Step 2: Install LCD driver
Step 3: Select board LOLIN(Wemos) D1 R1
*/
// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
//const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int rs = 0, en = 2, d4 = 4, d5 = 14, d6 = 12, d7 = 13; // for wemos d1 esp8266 + lcd robot shield d1
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// define newwords
const char* newwordTable[] = { 
  "plagiarism", 
  "nitpicking",
  "lenient",
  "boblehead",
  "fight-or-flight",
  "jeopardize",
  "quarrel",
  "ingrained",
  "nightfall",
  "fragile",
  "equilibrium" };

const char* newwordTable1[] = { 
  "dao van",  //plagiarism 
  "soi moi", //nitpicking
  "nhan hau",  //"lenient",
  "dau mo mong",
  "danh hay tranh",
  "gay hai",
  "cai nhau",
  "khac sau",
  "chieu toi",
  "mong manh",
  "can bang" };

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, new world!");
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis() / 1000);

  int cnt = 0;
  while(1)
  {
    if(newwordTable[cnt] != 0)
    {
      lcd.setCursor(0, 0);
      lcd.print(newwordTable[cnt]);
      lcd.setCursor(0, 1);
      lcd.print(newwordTable1[cnt]);
      cnt++;
      delay(3000);
      lcd.clear();
    }
    else
      cnt = 0;
  }
}

