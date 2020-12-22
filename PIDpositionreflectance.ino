boolean KP =  0.01;
boolean  KD  = 0.05;
#include <QTRSensors.h>
QTRSensors qtr;
const uint8_t SensorCount = 4;
uint16_t sensorValues[SensorCount];
int Rmotor= 12;
int RmotorPWM= 13;
int Lmotor= 8;
int LmotorPWM= 5;

void setup()
{
  for (int i = 2; i < 6; i++){ pinMode(i, INPUT);}
  for (int i = 10; i < 18; i++) {pinMode(i, OUTPUT);}
  // configure the sensors
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){3, 4, 5, 6}, SensorCount);
  //qtr.setEmitterPin(2);
  delay(500);
  Serial.begin(115200);
  Serial.println("Begin calibrations");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // turn on Arduino's LED to indicate we are in calibration mode
  // 2.5 ms RC read timeout (default) * 10 reads per calibrate() call
  // = ~25 ms per calibrate() call.
  // Call calibrate() 400 times to make calibration take about 10 seconds.
  for (uint16_t i = 0; i < 400; i++)
  {
    qtr.calibrate();
    //Have the robot self-calibrate
    if(i <150){
      turn_right();
      delay(10);
    }
    else{
      turn_left();
      delay(10);
    }
  }
  digitalWrite(LED_BUILTIN, LOW); // turn off Arduino's LED to indicate we are through with calibration
  // print the calibration minimum values measured when emitters were on
  Serial.println("printing minimum");
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(qtr.calibrationOn.minimum[i]);
    Serial.print(' ');
  }
  Serial.println();
  // print the calibration maximum values measured when emitters were on
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(qtr.calibrationOn.maximum[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
  delay(1000);

  pinMode(Rmotor,OUTPUT);
  pinMode(Lmotor,OUTPUT);
  pinMode(RmotorPWM,OUTPUT);
  pinMode(LmotorPWM, OUTPUT);
 
}
int lastError = 0;


void loop()
{
  // read calibrated sensor values and obtain a measure of the line position
  // from 0 to 5000 (for a white line, use readLineWhite() instead)
  uint16_t position = qtr.readLineBlack(sensorValues);
  // print the sensor values as numbers from 0 to 1000, where 0 means maximum
  // reflectance and 1000 means minimum reflectance, followed by the line
  // position
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
    if (sensorValues[i] > 800) {digitalWrite(i + 10, HIGH);}
    else {digitalWrite(i+10, LOW);}
  }

  Serial.println(position);
  int error = position - 1500;
  double motorSpeed = KP * error + KD *(error-lastError);//change in PWM(negative if turning left, positive if turning right)
  lastError = error;
  // the next if statement not really necessary just printing out values and comparing it to previous code
   if ((position>= 1000) && (position <= 2000)){
    Serial.println("Straight");
    Serial.println(motorSpeed);
  }
  else if (position > 2000){
    Serial.println("right");
    Serial.println(motorSpeed); 
  }
  else if (position < 1000){
    Serial.println("left");
    Serial.println(motorSpeed);
  }
  else{
     Serial.println("Straight");
     Serial.println(motorSpeed);
  }
  move(motorSpeed);

  delay(100);
}

//functions for moving
void turn_right(){
  digitalWrite(Rmotor,HIGH);
  digitalWrite(Lmotor,HIGH);
  analogWrite(RmotorPWM,90);
  analogWrite(LmotorPWM, 50);
}
void turn_left(){
  digitalWrite(Rmotor,HIGH);
  digitalWrite(Lmotor,HIGH);
  analogWrite(RmotorPWM, 50);
  analogWrite(LmotorPWM, 90);
} 

void move(int motorSpeed){
   digitalWrite(Rmotor,HIGH);
  digitalWrite(Lmotor,HIGH);
  analogWrite(RmotorPWM,60+motorSpeed);
  analogWrite(LmotorPWM, 60 + motorSpeed);
}
