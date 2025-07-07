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
#include <HCSR04.h>    //library: https://github.com/Martinsos/arduino-lib-hc-sr04

#include "DFRobotDFPlayerMini_proc.h"

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
#define MT_RIGHT_ARM_INIT_ANGLE   45     //wave right arm
#define MT_LEFT_ARM_INIT_ANGLE    45
#define MT_NECK_ROT_INIT_ANGLE    90     //rotate a neck
#define MT_NECK_NOD_INIT_ANGLE    90     //nod

#define MT_RIGHT_ARM_MAX_ANGLE    120     //max angle to keep safe
#define MT_LEFT_ARM_MAX_ANGLE     120
#define MT_NECK_ROT_MAX_ANGLE     135     //rotate a neck
#define MT_NECK_NOD_MAX_ANGLE     150     //nod

#define MT_RIGHT_ARM_MIN_ANGLE    MT_RIGHT_ARM_INIT_ANGLE
#define MT_LEFT_ARM_MIN_ANGLE     MT_LEFT_ARM_INIT_ANGLE
#define MT_NECK_ROT_MIN_ANGLE     30 
#define MT_NECK_NOD_MIN_ANGLE     MT_NECK_NOD_INIT_ANGLE

void rb_init_position();
void rb_ready_state();
void rb_eye_ctrl(int eye_pos,int eye_mode);

void mt_rot_degrees(int mtIdx, int degrees);
void mt_rot_degrees_speed(int mtIdx, int start_degree, int stop_degree, int delay_time); 
void mt_self_test();

//led
void led_test() ;

const int mt_info[4][2] = 
{
  {MT_RIGHT_ARM,MT_RIGHT_ARM_INIT_ANGLE},
  {MT_LEFT_ARM,MT_LEFT_ARM_INIT_ANGLE},
  {MT_NECK_ROT,MT_NECK_ROT_INIT_ANGLE},
  {MT_NECK_NOD,MT_NECK_NOD_INIT_ANGLE},
};

//Define IO
#define SONAR_TRIGGER_PIN 4
#define SONAR_ECHO_PIN    5
#define LED_RIGHT_EYE_PIN	6
#define LED_LEFT_EYE_PIN	7

#define EYE_CTRL_ON  HIGH
#define EYE_CTRL_OFF LOW
#define EYE_RIGHT 0
#define EYE_LEFT  1
#define EYE_ALL   2

UltraSonicDistanceSensor distanceSensor(SONAR_TRIGGER_PIN, SONAR_ECHO_PIN);  // Initialize sonar sensor

void setup() {
  Serial.begin(9600);
  Serial.println("Wall-E ...!");

  //Pin out for LEDs
  pinMode(LED_RIGHT_EYE_PIN, OUTPUT);
  pinMode(LED_LEFT_EYE_PIN, OUTPUT);

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

  //Setup MP3 player
  setup_DFRobotDFPlayerMini();

  //Do self test
  rb_self_test();

  rb_ready_state();

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
  mt_rot_degrees(MT_RIGHT_ARM,MT_RIGHT_ARM_INIT_ANGLE);
  mt_rot_degrees(MT_LEFT_ARM,MT_LEFT_ARM_INIT_ANGLE);
  mt_rot_degrees(MT_NECK_ROT,MT_NECK_ROT_INIT_ANGLE); //90
  mt_rot_degrees(MT_NECK_NOD,MT_NECK_NOD_INIT_ANGLE); //45  
}

void rb_ready_state()
{
  rb_eye_ctrl(EYE_ALL,EYE_CTRL_ON);
  rb_init_position();
}

//Run all motors
void mt_self_test()
{
  // mt_rot_degrees(MT_RIGHT_ARM,120);   //down from 180 to 120 due to hardware limitation
  // mt_rot_degrees(MT_LEFT_ARM,120);    //when the hands are moving over 150, it will be stucked by its head=>make motor broken
  // delay(1000);
  // mt_init_position(MT_RIGHT_ARM);
  // mt_init_position(MT_LEFT_ARM);

  //Test 2 arms up and down
  mt_rot_degrees_speed(MT_RIGHT_ARM,MT_RIGHT_ARM_INIT_ANGLE,90,20);
  mt_rot_degrees_speed(MT_RIGHT_ARM,90,MT_RIGHT_ARM_INIT_ANGLE,20);

  mt_rot_degrees_speed(MT_LEFT_ARM,MT_LEFT_ARM_INIT_ANGLE,90,20);
  mt_rot_degrees_speed(MT_LEFT_ARM,90,MT_LEFT_ARM_INIT_ANGLE,20);

  //mt_rot_degrees_speed(MT_LEFT_ARM,45,120,15);
  //mt_rot_degrees_speed(MT_LEFT_ARM,120,45,15);
  //while(1);

  //Rotate head to left and right
  //mt_rot_degrees(MT_NECK_ROT,0);
  mt_rot_degrees_speed(MT_NECK_ROT,MT_NECK_ROT_INIT_ANGLE,0,15);
  //delay(750);
  mt_rot_degrees_speed(MT_NECK_ROT,0,180,10);
  //delay(750);
  mt_rot_degrees_speed(MT_NECK_ROT,180,MT_NECK_ROT_INIT_ANGLE,15);
  delay(750);

  int cnt = 1;
  //head down 
  while(cnt--){
  mt_rot_degrees_speed(MT_NECK_NOD,MT_NECK_NOD_INIT_ANGLE,120,15);
  //delay(750);
  mt_rot_degrees_speed(MT_NECK_NOD,120,MT_NECK_NOD_INIT_ANGLE,15);
  //delay(750);
  }
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
    mt_rot_degrees_speed(MT_RIGHT_ARM,75,100,5);
    mt_rot_degrees_speed(MT_RIGHT_ARM,100,75,5);
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
  int stop_degree = 110;
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

void rb_nod(int nodCnt)
{
  while(nodCnt--)
  {
    mt_rot_degrees_speed(MT_NECK_NOD,MT_NECK_NOD_INIT_ANGLE,60,10);
    //delay(750);
    mt_rot_degrees_speed(MT_NECK_NOD,60,MT_NECK_NOD_INIT_ANGLE,10); 
  }
}

//Read sonar sensor when rotate neck, look around
//If detect something, say fun world, do arm gesture
enum sys_state_e
{
  S_INIT,
  S_DETECTED_PEOPLE,
  S_SCANNING
}_SYS_STATE_E;

sys_state_e gState = S_INIT;


void led_test() {
  digitalWrite(LED_RIGHT_EYE_PIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(500); 
  digitalWrite(LED_RIGHT_EYE_PIN, LOW);   // turn the LED off by making the voltage LOW
  
  digitalWrite(LED_LEFT_EYE_PIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(500);                      // wait for a second
  digitalWrite(LED_LEFT_EYE_PIN, LOW);   // turn the LED off by making the voltage LOW 
  delay(500);                      // wait for a second
}

void distance_sensor_test()
{
  Serial.println(distanceSensor.measureDistanceCm());
}

void rb_self_test()
{
  //LED (robot eyes)
  led_test();
  //All servos
  mt_self_test();
  //Distance sensor
  distance_sensor_test();
  //play mp3
  DFPlayer_test();
}

void rb_eye_wink(int eye_pos,int wink_cnt, int wink_delay)
{ 
  int led_pin = LED_LEFT_EYE_PIN;
  if(eye_pos== EYE_RIGHT)
  {
    led_pin = LED_RIGHT_EYE_PIN;
  }

  while(wink_cnt--)
  {
    digitalWrite(led_pin, HIGH);
    delay(200);
    digitalWrite(led_pin, LOW);
    delay(200);
  }
}

void rb_eye_ctrl(int eye_pos,int eye_mode)
{ 
  switch(eye_pos)  
  {
    case EYE_RIGHT: //right
      digitalWrite(LED_RIGHT_EYE_PIN, eye_mode);  // turn the LED on (HIGH is the voltage level)
      break;
    case EYE_LEFT: //left
      digitalWrite(LED_LEFT_EYE_PIN, eye_mode);  // turn the LED on (HIGH is the voltage level)
      break;
    case EYE_ALL: //all
      digitalWrite(LED_RIGHT_EYE_PIN, eye_mode);  // turn the LED on (HIGH is the voltage level)
      digitalWrite(LED_LEFT_EYE_PIN, eye_mode);  // turn the LED on (HIGH is the voltage level)
      break;
    defautl:
      break;
  }
}

void loop() {
  //Stop robot temporally
  while(1);

  rb_suveilance();

  //Wave arm
  //rb_wave_arm();

  //Show happy mode
  //rb_happy();

  //Stop loop for testing
  //while(1);
}

int gHeadRotAngle = MT_NECK_ROT_INIT_ANGLE;

void rb_suveilance()
{
  static int headAngle = gHeadRotAngle; //get a current head rotation angle position
  static bool  bScanDirect = true;
  int measureDistance = distanceSensor.measureDistanceCm();
  Serial.println(measureDistance); //debug


  if (measureDistance > 0 && measureDistance < 30 && gState != S_DETECTED_PEOPLE)
  {
    gState = S_DETECTED_PEOPLE;
  }

  switch(gState)
  {
      case S_INIT:
        gState = S_SCANNING;
        break;
      
      case S_DETECTED_PEOPLE:
        Serial.println("Hello human");
        rb_eye_wink(EYE_LEFT,2,200);
        //rb_say_hello
        rb_eye_ctrl(EYE_LEFT,EYE_CTRL_ON); //back to last state
        rb_wave_arm();
        rb_nod(1);
        gState = S_SCANNING;

        break;
      
      // case S_CHECK_PEOPLE:
      //   if (measureDistance > 0 && measureDistance < 30 && gState != S_DETECTED_PEOPLE)
      //   {
      //     gState = S_DETECTED_PEOPLE;
      //   }
      //   else
      //   {
      //     gState = S_SCANNING;
      //   }
      //   break;

      case S_SCANNING:
        measureDistance = distanceSensor.measureDistanceCm();
        Serial.println(measureDistance); //debug

        if (measureDistance > 0 && measureDistance < 30)
        {
          gState = S_DETECTED_PEOPLE;
          break;
        }

        if(bScanDirect == true){
          if(headAngle < MT_NECK_ROT_MAX_ANGLE)
            mt_rot_degrees(MT_NECK_ROT,headAngle++);
          else
            bScanDirect = false; //revert rotation direction
            //headAngle = MT_NECK_ROT_MIN_ANGLE; //reset
        }else
        {
          if(headAngle > MT_NECK_ROT_MIN_ANGLE)
            mt_rot_degrees(MT_NECK_ROT,headAngle--);
          else
            bScanDirect = true; //revert rotation direction
        }

        Serial.print("Head rot angle:"); //debug
        Serial.println(headAngle);

        break;
      default:
        break;
  }

  delay(100);
}
