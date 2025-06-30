/*************************************************** 
  This is an example for our Adafruit 16-channel PWM & Servo driver
  Servo test - this will drive 8 servos, one after the other on the
  first 8 pins of the PCA9685

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815
  
  These drivers use I2C to communicate, 2 pins are required to  
  interface.

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);
// you can also call it with a different address and I2C interface
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  100 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  450 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

// our servo # counter
uint8_t servonum = 0;

#define MT_RIGHT_ARM  0     //wave right arm
#define MT_LEFT_ARM   1
#define MT_NECK_ROT   2     //rotate a neck
#define MT_NECK_NOD   3     //nod

//Init angle
#define MT_RIGHT_ARM_INIT_ANGLE  0     //wave right arm
#define MT_LEFT_ARM_INIT_ANGLE   0
#define MT_NECK_ROT_INIT_ANGLE   90     //rotate a neck
#define MT_NECK_NOD_INIT_ANGLE   45     //nod

void rb_init_position();
void mt_rot_degrees(int mtIdx, int degrees);
void mt_rot_degrees_speed(int mtIdx, int start_degree, int stop_degree, int delay_time); 

const int mt_info[4][2] = 
{
  {MT_RIGHT_ARM,MT_RIGHT_ARM_INIT_ANGLE},
  {MT_LEFT_ARM,MT_LEFT_ARM_INIT_ANGLE},
  {MT_NECK_ROT,MT_NECK_ROT_INIT_ANGLE},
  {MT_NECK_NOD,MT_NECK_NOD_INIT_ANGLE},
};

void setup() {
  Serial.begin(9600);
  Serial.println("8 channel Servo test!");

  pwm.begin();
  /*
   * In theory the internal oscillator (clock) is 25MHz but it really isn't
   * that precise. You can 'calibrate' this by tweaking this number until
   * you get the PWM update frequency you're expecting!
   * The int.osc. for the PCA9685 chip is a range between about 23-27MHz and
   * is used for calculating things like writeMicroseconds()
   * Analog servos run at ~50 Hz updates, It is importaint to use an
   * oscilloscope in setting the int.osc frequency for the I2C PCA9685 chip.
   * 1) Attach the oscilloscope to one of the PWM signal pins and ground on
   *    the I2C PCA9685 chip you are setting the value for.
   * 2) Adjust setOscillatorFrequency() until the PWM update frequency is the
   *    expected value (50Hz for most ESCs)
   * Setting the value here is specific to each individual I2C PCA9685 chip and
   * affects the calculations for the PWM update frequency. 
   * Failure to correctly set the int.osc value will cause unexpected PWM results
   */
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  delay(10);

  rb_init_position();
}

// You can use this function if you'd like to set the pulse length in seconds
// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. It's not precise!
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= SERVO_FREQ;   // Analog servos run at ~60 Hz updates
  Serial.print(pulselength); Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit"); 
  pulse *= 1000000;  // convert input seconds to us
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}

//motor rotate to absolute angle
void mt_rot_degrees(int mtIdx, int degrees) //absolute degree from 0 to 180
{
  if(mtIdx == MT_LEFT_ARM)
    degrees = 180 - degrees;

  int pulselen = map(degrees, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(mtIdx, 0, pulselen);
}

//Control the speed of motor rotation
void mt_rot_degrees_speed(int mtIdx, int start_degree, int stop_degree, int delay_time)
{
  if(stop_degree>= start_degree)
  {
    for(int degrees = start_degree;degrees <= stop_degree; degrees ++)
    {
      mt_rot_degrees(mtIdx, degrees);
      delay(delay_time);
    }
  }
  else
  {
    for(int degrees = start_degree;degrees >= stop_degree; degrees --)
    {
      mt_rot_degrees(mtIdx, degrees);
      delay(delay_time);
    }
  }
}

//Motor init state
void mt_init_position(int mtIdx)
{
  mt_rot_degrees(mtIdx, mt_info[mtIdx][1]);
}

//Robot init state
void rb_init_position()
{
  mt_rot_degrees(MT_RIGHT_ARM,0);
  mt_rot_degrees(MT_LEFT_ARM,0);
  mt_rot_degrees(MT_NECK_ROT,MT_NECK_ROT_INIT_ANGLE); //90
  mt_rot_degrees(MT_NECK_NOD,MT_NECK_NOD_INIT_ANGLE); //45  
}

//Run all motors
void rb_self_test()
{
  // mt_rot_degrees(MT_RIGHT_ARM,120);   //down from 180 to 120 due to hardware limitation
  // mt_rot_degrees(MT_LEFT_ARM,120);    //when the hands are moving over 150, it will be stucked by its head=>make motor broken
  // delay(1000);
  // mt_init_position(MT_RIGHT_ARM);
  // mt_init_position(MT_LEFT_ARM);

  //Test 2 arms up and down
  mt_rot_degrees_speed(MT_RIGHT_ARM,MT_RIGHT_ARM_INIT_ANGLE,45,10);
  mt_rot_degrees_speed(MT_RIGHT_ARM,45,MT_RIGHT_ARM_INIT_ANGLE,10);

  mt_rot_degrees_speed(MT_LEFT_ARM,MT_LEFT_ARM_INIT_ANGLE,45,10);
  mt_rot_degrees_speed(MT_LEFT_ARM,45,MT_LEFT_ARM_INIT_ANGLE,10);

  //Rotate head to left and right
  //mt_rot_degrees(MT_NECK_ROT,0);
  mt_rot_degrees_speed(MT_NECK_ROT,MT_NECK_ROT_INIT_ANGLE,0,10);
  //delay(750);
  mt_rot_degrees_speed(MT_NECK_ROT,0,180,10);
  //delay(750);
  mt_rot_degrees_speed(MT_NECK_ROT,180,MT_NECK_ROT_INIT_ANGLE,10);
  delay(750);

  //head down 
  mt_rot_degrees_speed(MT_NECK_NOD,MT_NECK_NOD_INIT_ANGLE,0,10);
  //delay(750);
  mt_rot_degrees_speed(MT_NECK_NOD,0,90,10);
  //delay(750);
  //while(1);

  //back to intial state
  rb_init_position(); 
}

//Wave arm
void rb_wave_arm()
{
  int wave_cnt = 3;

  //wave 3 times
  while(wave_cnt--)
  {
    mt_rot_degrees_speed(MT_RIGHT_ARM,75,135,5);
    mt_rot_degrees_speed(MT_RIGHT_ARM,135,75,5);
  }

  //arm down
  mt_rot_degrees_speed(MT_RIGHT_ARM,75,MT_RIGHT_ARM_INIT_ANGLE,5);
  //mt_init_position(MT_RIGHT_ARM);
  //delay(500);
}

void rb_happy()
{
  int wave_cnt = 3;
  int start_degree = 60;
  int stop_degree = 135;
  int sum_degree = start_degree +  stop_degree;

  //wave 3 times
  while(wave_cnt--)
  {

    //Two arm move revertly
    for(int degrees = start_degree;degrees <= stop_degree; degrees ++)
    {
      mt_rot_degrees(MT_RIGHT_ARM, degrees);
      mt_rot_degrees(MT_LEFT_ARM, sum_degree - degrees);

      //@todo nod
      delay(5);
    }
    delay(10);
  }

  //Two hand down
  rb_init_position();

}

//Read sonar sensor when rotate neck, look around
//If detect something, say fun world, do arm gesture
void rb_suveilance()
{

}

void loop() {
  //Stop robot temporally
  //while(1);

  //Do self test
  rb_self_test();

  //Wave arm
  //rb_wave_arm();

  //Show happy mode
  //rb_happy();

  //Stop loop for testing
  while(1);
}
