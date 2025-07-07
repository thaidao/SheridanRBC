/***************************************************
DFPlayer - A Mini MP3 Player For Arduino
 <https://www.dfrobot.com/product-1121.html>
 
 ***************************************************
 This example shows the basic function of library for DFPlayer.
 
 Created 2016-12-07
 By [Angelo qiao](Angelo.qiao@dfrobot.com)
 
 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/

/***********Notice and Trouble shooting***************
 1.Connection and Diagram can be found here
 <https://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299#Connection_Diagram>
 2.This code is tested on Arduino Uno, Leonardo, Mega boards.
 ****************************************************/

#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"

#ifndef DFPLAYER_H
#define DFPLAYER_H


// #if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))   // Using a soft serial port
// #include <SoftwareSerial.h>
// SoftwareSerial softSerial(/*rx =*/11, /*tx =*/10);
// #define FPSerial softSerial
// #else
// #define FPSerial Serial1
// #endif

//DFRobotDFPlayerMini myDFPlayer;
void DFPlayer_test();
void setup_DFRobotDFPlayerMini();

#endif
