#include <HCSR04.h>

//library: https://github.com/Martinsos/arduino-lib-hc-sr04

#define SONAR_TRIGGER_PIN 4
#define SONAR_ECHO_PIN    5
UltraSonicDistanceSensor distanceSensor(SONAR_TRIGGER_PIN, SONAR_ECHO_PIN);  // Initialize sensor that uses digital pins 13 and 12.

void setup () {
    Serial.begin(9600);  // We initialize serial connection so that we could print values from sensor.
}

void loop () {
    // Every 500 miliseconds, do a measurement using the sensor and print the distance in centimeters.
    Serial.println(distanceSensor.measureDistanceCm());
    delay(500);
}
