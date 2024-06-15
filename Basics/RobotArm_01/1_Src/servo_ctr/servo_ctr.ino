/*
MIT License
Copyright 2021 Michael Schoeffler (https://www.mschoeffler.com)
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/*
* Example source code of an Arduino tutorial on how to control an MG 996R servo motor. 
*/

#include <Servo.h> 

Servo servo_0; // servo object representing the MG 996R servo
Servo servo_1; // servo object representing the MG 996R servo
Servo servo_2; // servo object representing the MG 996R servo
Servo servo_3; // servo object representing the MG 996R servo
Servo servo_4; // servo object representing the MG 996R servo

// define the motor control pin
#define SVM_CTR_0 2 // servo is wired to Arduino on digital pin 2
#define SVM_CTR_1 3 // init angle 180
#define SVM_CTR_2 4
#define SVM_CTR_3 5
#define SVM_CTR_4 6

// define the motor index
#define SVM_0 0 
#define SVM_1 1
#define SVM_2 2
#define SVM_3 3
#define SVM_4 4

#define NUM_SERVO 5
const Servo* svm_table[NUM_SERVO]=
{
  &servo_0,&servo_1,&servo_2,&servo_3,&servo_4
};

// Store current angle of all servo
int ServoCurAnglePos[NUM_SERVO];

//servor table
//servor idx, intial angle, enable 
void setup() {
  servo_0.attach(SVM_CTR_0);
  servo_1.attach(SVM_CTR_1);
  servo_2.attach(SVM_CTR_2);
  servo_3.attach(SVM_CTR_3);
  servo_4.attach(SVM_CTR_4);
  Serial.begin(9600);
}

void svm_baby_step2(int servoIdx, int startAngle, int endAngle, int delayTime)
{
  Serial.print("servoIdx: ");
  Serial.print(servoIdx);

  Serial.print(" startAngle: ");
  Serial.print(startAngle);

  Serial.print(" endAngle: ");
  Serial.print(endAngle);
  Serial.println();

  int intialAngle = startAngle;
  int DIFF_ANGLE = 1;
  if(startAngle > endAngle)
  {
    intialAngle = startAngle;
    DIFF_ANGLE = -1;
  }
  else if (startAngle == endAngle)
  {
    svm_table[servoIdx]->write(startAngle);
    svm_update_cur_angle(servoIdx,startAngle);
    return;
  }

  int diffAngle = abs(startAngle-endAngle);
  Serial.print(" diffAngle: ");
  Serial.print(diffAngle);
  Serial.println();

  Serial.print("intialAngle: ");
  for(int step = 0; step < diffAngle; step ++)
  {

    Serial.print(intialAngle);
    Serial.print(",");

    svm_table[servoIdx]->write(intialAngle);
    svm_update_cur_angle(servoIdx,intialAngle);
    intialAngle += DIFF_ANGLE;
    delay(delayTime);
  }
  Serial.println();
}

void svm_baby_step(int servoIdx, int angle, int totalStep, int delayTime)
{
  int anglePerStep = 0;
  int startAngle =  svm_get_cur_angle(servoIdx);
  int nextAngle = 0;//startAngle; 
  int diffAngle = abs(angle-nextAngle);

  //Caculate angle for each step
  anglePerStep = diffAngle/totalStep;

  if(anglePerStep <= 1 )
  {
    anglePerStep = 1;
  }

  Serial.print("servoIdx: ");
  Serial.print(servoIdx);

  Serial.print(" startAngle: ");
  Serial.print(startAngle);

  if (angle < startAngle)
  {
    nextAngle = angle;
  }

  Serial.print(" nextAngle: ");
  Serial.print(nextAngle);

  Serial.println();

  for(int cnt = 0; cnt < totalStep; cnt++)
  {
    // Serial.print("cnt=");
    // Serial.print(cnt);
    // Serial.println();
    nextAngle += anglePerStep;

    // Serial.print("nextAngle=");
    // Serial.print(nextAngle);
    // Serial.println();
    svm_table[servoIdx]->write(nextAngle);
    //Update currnt pos
    svm_update_cur_angle(servoIdx,nextAngle);
    delay(delayTime);
  }
}

//Control servo by angle
void svm_ctr_rotate(int servoIdx, int angle, int totalStep, int delayTime)
{
  if(servoIdx > NUM_SERVO - 1 || angle < 0 || angle > 180 || totalStep < 0 || delayTime < 0){
    return;
  }

  //int start_angle = svm_get_cur_angle(servoIdx);

  svm_baby_step(servoIdx, angle, totalStep, delayTime);
}

//Control servo by angle
void svm_ctr_rotate2(int servoIdx, int angle, int totalStep, int delayTimeEachStep)
{
  if(servoIdx > NUM_SERVO - 1 || angle < 0 || angle > 180 || totalStep < 0 || delayTimeEachStep < 0){
    return;
  }

  int startAngle = svm_get_cur_angle(servoIdx);
  svm_baby_step2(servoIdx,startAngle,angle, delayTimeEachStep);
}

//Save the latest angle
void svm_update_cur_angle(int servoIdx, int curAngle)
{
  ServoCurAnglePos[servoIdx] = curAngle;
}

int svm_get_cur_angle(int servoIdx)
{
  return ServoCurAnglePos[servoIdx];
}


//Set init state
// void svm_init_state()
// {
//   //Reset all motor to intial postion
//   for(int cnt = 0; cnt < NUM_SERVO; cnt ++)
//   {
//     //@todo: improve code, more flexible for intial state
//     svm_ctr_rotate(svm_table[cnt],0,1,200);
//   }
// }

void svm_init_state()
{
  // svm_table[SVM_0]->write(0);
  // delay(1000);
  // svm_table[SVM_1]->write(45);
  // delay(1000);
  // svm_table[SVM_2]->write(135);
  // delay(1000);
  // svm_table[SVM_3]->write(0);
  // delay(1000);
  // svm_table[SVM_4]->write(90);
  // delay(1000);

  // svm_ctr_rotate(SVM_4,90,45,100);
  // delay(1000);
  // svm_ctr_rotate(SVM_3,0,45,100);
  // delay(1000);
  // svm_ctr_rotate(SVM_2,135,45,100);
  // delay(1000);
  // svm_ctr_rotate(SVM_1,45,45,100);
  // delay(1000);
  // svm_ctr_rotate(SVM_0,0,45,200);
  // delay(1000);

  // svm_ctr_rotate(SVM_4,90,45,100);
  // delay(1000);
  // svm_ctr_rotate(SVM_3,0,45,100);
  // delay(1000);
  // svm_ctr_rotate(SVM_2,90,45,100);
  // delay(1000);
//while(1);
{


#if 0 //test servo 2 okay
  // svm_ctr_rotate2(SVM_4,0,45,50);
  // delay(1000);
  // svm_ctr_rotate2(SVM_4,45,45,50);
  // delay(1000);
  // svm_ctr_rotate2(SVM_4,90,45,50);
  // delay(1000);
  // svm_ctr_rotate2(SVM_4,180,45,50);
  // delay(1000);
  // svm_ctr_rotate2(SVM_4,90,45,50);
  // delay(1000);
  // svm_ctr_rotate2(SVM_4,45,45,50);
  // delay(1000);
  // svm_ctr_rotate2(SVM_4,0,45,50);
  // delay(1000);
  // svm_ctr_rotate2(SVM_4,90,45,50);
  // delay(1000);
#endif

  while(1)
  {
    int svmIdx = SVM_2;
    svm_ctr_rotate2(svmIdx,150,1000,50);
    delay(1000);
    // svm_ctr_rotate2(svmIdx,45,1000,50);
    // delay(1000);
    // svm_ctr_rotate2(svmIdx,180,1000,50);
    // delay(3000);
    // svm_ctr_rotate2(svmIdx,45,1000,50);
    // delay(1000);
    // svm_ctr_rotate2(svmIdx,0,1000,50);
    // delay(1000);
  }
}

  // svm_ctr_rotate(SVM_0,0,45,200);
  // delay(1000);
}

void svm_selftest_simple()
{
  svm_table[SVM_0]->write(0);
  delay(1000);
  svm_table[SVM_1]->write(0);
  delay(1000);
  svm_table[SVM_2]->write(0);
  delay(1000);
  svm_table[SVM_3]->write(0);
  delay(1000);
  svm_table[SVM_4]->write(0);
  delay(1000);
}

//Self rotate all motor from zero to 180 degree
void svm_selftest_all()
{
  for(int svm_idx = 0; svm_idx < NUM_SERVO ; svm_idx ++)
  {
    for(int angle = 0; angle <=180; angle += 45)
    {
      svm_table[svm_idx]->write(angle);
      delay(1000);
    }
    //Reset to zero degree after finshing the test.
    svm_table[svm_idx]->write(0);
  }
}

//main program
void loop()
{
  Serial.println("Robot arm's program");
  // svm_table[SVM_4]->write(0);
  // delay(1000);
  // svm_table[SVM_4]->write(90);
  // delay(1000);

  // servo_0.write(135);
  // delay(1000);
  // servo_0.write(0);
  // delay(1000);

// while(1)
// {
//   svm_ctr_rotate(SVM_0,135,50,100);
//   delay(1000);
//   svm_ctr_rotate(SVM_0,0,50,100);
//   delay(1000);
// }
  //Init state
  svm_init_state();

  //Set all intial angle of servo are zero
  //svm_selftest_simple();

  //Self rotate all motor from zero to 180 degree
  //svm_selftest_all();

  //main function
  while(1);
}

void svm_self_test() {
  servo_0.write(0); // move MG996R's shaft to angle 0°
  delay(1000); // wait for one second
  servo_0.write(45); // move MG996R's shaft to angle 45°
  delay(1000); // wait for one second 
  servo_0.write(90); // move MG996R's shaft to angle 90°
  delay(1000); // wait for one second
  servo_0.write(135); // move MG996R's shaft to angle 135°
  delay(1000); // wait for one second
  servo_0.write(180); // move MG996R's shaft to angle 180°
  delay(1000); // wait for one second
}