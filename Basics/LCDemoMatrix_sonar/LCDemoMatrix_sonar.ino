//We always have to include the library
#include "LedControl.h"
#include <HCSR04.h>

// define pin for ultrasonic sensor
byte triggerPin = 6;
byte echoPin = 5;

// Get distance from ultrasonic sensor
double get_distance () {
  double* distances = HCSR04.measureDistanceCm();
  return distances[0];
}

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 4 is connected to the DataIn 
 pin 2 is connected to the CLK 
 pin 3 is connected to LOAD 
 We have only a single MAX72XX.
 */
LedControl lc=LedControl(4,2,3,1);

/*
Tool for generat 
https://www.riyas.org/2013/12/online-led-matrix-font-generator-with.html
8x8 ONLINE LED HEX/ BINARY PATTERN GENERATOR FOR ARDUINO
*/

/* we always wait a bit between updates of the display */
unsigned long delaytime=500;

void setup() {
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);

  /* Initialize ultrasonic sensor */
  HCSR04.begin(triggerPin, echoPin);

  /* Intialize serial */
  Serial.begin(9600);
}

/*
 This method will display the characters for the
 word "Arduino" one after the other on the matrix. 
 (you need at least 5x7 leds to see the whole chars)
 */
void writeArduinoOnMatrix() {
  /* here is the data for the characters */
  byte a[5]={B01111110,B10001000,B10001000,B10001000,B01111110};
  byte r[5]={B00111110,B00010000,B00100000,B00100000,B00010000};
  byte d[5]={B00011100,B00100010,B00100010,B00010010,B11111110};
  byte u[5]={B00111100,B00000010,B00000010,B00000100,B00111110};
  byte i[5]={B00000000,B00100010,B10111110,B00000010,B00000000};
  byte n[5]={B00111110,B00010000,B00100000,B00100000,B00011110};
  byte o[5]={B00011100,B00100010,B00100010,B00100010,B00011100};
  byte s[8]={B00011000,B00100100,B00100000,B00011000,B00000100,B00000100,B00100100,B00011000};
  byte heart[8]={B00000000,
  B00000000,
  B00110110,
  B01111111,
  B01111111,
  B00111110,
  B00011100,
  B00001000};

  byte reverted_heart[8]=
  {B11111111,
  B11111111,
  B11001001,
  B10000000,
  B10000000,
  B11000001,
  B11100011,
  B11110111};

  /* now display them one by one with a small delay */
  lc.setRow(0,0,heart[0]);
  lc.setRow(0,1,heart[1]);
  lc.setRow(0,2,heart[2]);
  lc.setRow(0,3,heart[3]);
  lc.setRow(0,4,heart[4]);
  lc.setRow(0,5,heart[5]);
  lc.setRow(0,6,heart[6]);
  lc.setRow(0,7,heart[7]);
  delay(delaytime);
  lc.setRow(0,0,reverted_heart[0]);
  lc.setRow(0,1,reverted_heart[1]);
  lc.setRow(0,2,reverted_heart[2]);
  lc.setRow(0,3,reverted_heart[3]);
  lc.setRow(0,4,reverted_heart[4]);
  lc.setRow(0,5,reverted_heart[5]);
  lc.setRow(0,6,reverted_heart[6]);
  lc.setRow(0,7,reverted_heart[7]);
  delay(delaytime);
}

/*
  This function lights up a some Leds in a row.
 The pattern will be repeated on every row.
 The pattern will blink along with the row-number.
 row number 4 (index==3) will blink 4 times etc.
 */
void rows() {
  for(int row=0;row<8;row++) {
    delay(delaytime);
    lc.setRow(0,row,B10100000);
    delay(delaytime);
    lc.setRow(0,row,(byte)0);
    for(int i=0;i<row;i++) {
      delay(delaytime);
      lc.setRow(0,row,B10100000);
      delay(delaytime);
      lc.setRow(0,row,(byte)0);
    }
  }
}

/*
  This function lights up a some Leds in a column.
 The pattern will be repeated on every column.
 The pattern will blink along with the column-number.
 column number 4 (index==3) will blink 4 times etc.
 */
void columns() {
  for(int col=0;col<8;col++) {
    delay(delaytime);
    lc.setColumn(0,col,B10100000);
    delay(delaytime);
    lc.setColumn(0,col,(byte)0);
    for(int i=0;i<col;i++) {
      delay(delaytime);
      lc.setColumn(0,col,B10100000);
      delay(delaytime);
      lc.setColumn(0,col,(byte)0);
    }
  }
}

/* 
 This function will light up every Led on the matrix.
 The led will blink along with the row-number.
 row number 4 (index==3) will blink 4 times etc.
 */
void single() {
  for(int row=0;row<8;row++) {
    for(int col=0;col<8;col++) {
      delay(delaytime);
      lc.setLed(0,row,col,true);
      delay(delaytime);
      for(int i=0;i<col;i++) {
        lc.setLed(0,row,col,false);
        delay(delaytime);
        lc.setLed(0,row,col,true);
        delay(delaytime);
      }
    }
  }
}

void test_sonar()
{
  double distance = 0;

  Serial.println("Testing the sonar sensor");
  while(1)
  {
    // Get distance
    distance = get_distance();
    Serial.print(distance);
    Serial.println(" cm");
    delay(200);
  }
}

void test_led_matrix()
{
  Serial.println("Testing LED matrix");
  while(1)
  {
    writeArduinoOnMatrix();
    delay(200);
    Serial.println("Testing LED matrix");
  }
}

void loop() { 

  double distance = 0;

  //Test 
  //test_sonar();

  //Test matrix
  test_led_matrix();

  while(1)
  {
    // Get distance
    distance = get_distance();
    Serial.print(distance);
    Serial.println(" cm");

    if(distance < 250 && distance > 0)
    {
      delay(100);
      // Change the led matrix blink frequency
      delaytime = distance*10;
      //continue;
    }
    else
      delaytime = 500;
    
    writeArduinoOnMatrix();

    delay(delaytime);

  //rows();
  //columns();
  //single();
  }
}
