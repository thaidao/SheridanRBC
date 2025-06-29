#include "FaBoPWM_PCA9685.h"

FaBoPWM faboPWM;
int pos = 0;
int MAX_VALUE = 600;
int MIN_VALUE = 300;

#define MT_RIGHT_ARM  0     ;wave right arm
#define MT_LEFT_ARM   1
#define MT_NECK_ROT   2     ;rotate a neck
#define MT_NECK_NOD   3     ;nod

void setup() {
  Serial.begin(115200);
  if(faboPWM.begin()) {
    Serial.println("Find PCA9685");
    faboPWM.init(300);
  }
  faboPWM.set_hz(50);
  
}

int g_motor_idx = 12;
void loop() {
  //while(1)
  {
    faboPWM.set_channel_value(0, 0);
    faboPWM.set_channel_value(1, 180); 
    faboPWM.set_channel_value(2, 0); 
    faboPWM.set_channel_value(3, 0); 
    delay(3000);
  }
  while(1)
  {
  for (pos = MIN_VALUE; pos <= MAX_VALUE; pos += 1) { 
    faboPWM.set_channel_value(0, pos);
    faboPWM.set_channel_value(1, pos); 
    faboPWM.set_channel_value(2, pos); 
    faboPWM.set_channel_value(3, pos);  
    delay(1);                      
  }
  for (pos = 600; pos >= MIN_VALUE; pos -= 1) { 
    faboPWM.set_channel_value(0, pos);
    faboPWM.set_channel_value(1, pos); 
    faboPWM.set_channel_value(2, pos); 
    faboPWM.set_channel_value(3, pos);             
    delay(1);      
  }
  }
  g_motor_idx ++;
  if(g_motor_idx == 4)
    g_motor_idx = 0;
}
