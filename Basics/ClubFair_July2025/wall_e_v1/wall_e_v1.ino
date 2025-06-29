#include "FaBoPWM_PCA9685.h"

FaBoPWM faboPWM;
int pos = 0;
int MAX_VALUE = 1200;
int MIN_VALUE = 0;

#define MT_RIGHT_ARM  0     //wave right arm
#define MT_LEFT_ARM   1
#define MT_NECK_ROT   2     //rotate a neck
#define MT_NECK_NOD   3     //nod

void rb_init();

void setup() {
  Serial.begin(115200);
  if(faboPWM.begin()) {
    Serial.println("Find PCA9685");
    faboPWM.init(300);
  }
  faboPWM.set_hz(50);

  rb_init();
  
}

int g_motor_idx = 12;

void rb_init()
{
    faboPWM.set_channel_value(MT_RIGHT_ARM, 0);
    faboPWM.set_channel_value(MT_LEFT_ARM, 0); 
    faboPWM.set_channel_value(MT_NECK_ROT, 0); 
    faboPWM.set_channel_value(MT_NECK_NOD, 0); 
}

void rb_wave_arm()
{
  faboPWM.set_channel_value(MT_RIGHT_ARM, 0);
  
  delay(1000);
//while(1);
  int wave_cnt = 3;
  int apos = 0;

  //wave 3 times
  while(wave_cnt--)
  {
    for (apos = 400; apos <= 600; apos ++)
    {
      faboPWM.set_channel_value(MT_RIGHT_ARM, apos);
      delay(1);
    }
  }

  //arm down
    for (apos = 400; apos >= 250; apos --)
    {
      faboPWM.set_channel_value(MT_RIGHT_ARM, apos);
      delay(1);
    }
}

void rb_happy()
{
    int apos = 0;
    int lpos;// = 200;
    int cnt = 100;
    while(cnt --)
    {
    lpos = 200;
    for (apos = 200; apos <= 400; apos ++)
    {
      faboPWM.set_channel_value(MT_RIGHT_ARM, apos);
      faboPWM.set_channel_value(MT_LEFT_ARM, lpos--);
      delay(1);
    }

    lpos = 0;
    for (apos = 400; apos >= 200; apos --)
    {
      faboPWM.set_channel_value(MT_RIGHT_ARM, apos);
      faboPWM.set_channel_value(MT_LEFT_ARM, lpos++);
      delay(1);
    }
    }

    // for (apos = 200; apos >= 0; apos --)
    // {
    //   faboPWM.set_channel_value(MT_LEFT_ARM, apos);
    //   delay(1);
    // }
}

void loop() {

  //rb_wave_arm();
  rb_happy();

  while(1);

  // while(1)
  // {
  //   faboPWM.set_channel_value(0, 0);
  //   faboPWM.set_channel_value(1, 180); 
  //   faboPWM.set_channel_value(2, 0); 
  //   faboPWM.set_channel_value(3, 0); 
  //   delay(3000);
  // }

  while(1)
  {
  for (pos = 400; pos <= 600; pos += 1) { 
    faboPWM.set_channel_value(0, pos);
    faboPWM.set_channel_value(1, pos); 
    faboPWM.set_channel_value(2, pos); 
    faboPWM.set_channel_value(3, pos);  
    delay(1);                      
  }
  for (pos = 600; pos >= 400; pos -= 1) { 
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
