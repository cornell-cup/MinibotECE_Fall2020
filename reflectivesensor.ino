#include <QTRSensors.h>
#include <Wire.h>
QTRSensors qtr;
const uint8_t SensorCount = 4;//use four sensors
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
  }
  digitalWrite(LED_BUILTIN, LOW); // turn off Arduino's LED to indicate we are through with calibration

  // print the calibration minimum values measured when emitters were on
  //Serial.begin(9600);
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

  }
  Serial.println(position);
//use sensor values to determine where line is and move accordingly
  if ((sensorValues[1] > 500) || (sensorValues[2]> 500)){
    Serial.println("Straight");
    drive_forward();
  }
  else if (sensorValues[0] > 500){
    Serial.println("right");
    right(); 
  }
  else if (sensorValues[3] > 500){
    Serial.println("left");
    left();
  }
  else{
    Serial.println("Straight");
    drive_forward();
  }

  delay(250);
}
//functions for driving car
/*
void steep_left(){
  digitalWrite(Rmotor,HIGH);
  digitalWrite(Lmotor,HIGH);
  analogWrite(RmotorPWM,50);
  analogWrite(LmotorPWM, 140);
}
*/

void left(){
  digitalWrite(Rmotor,HIGH);
  digitalWrite(Lmotor,HIGH);
  analogWrite(RmotorPWM,50);
  analogWrite(LmotorPWM, 90);
}

/*
void steep_right(){
  digitalWrite(Rmotor,HIGH);
  digitalWrite(Lmotor,HIGH);
  analogWrite(RmotorPWM,140);
  analogWrite(LmotorPWM, 50);
}
*/

void right(){
  digitalWrite(Rmotor,HIGH);
  digitalWrite(Lmotor,HIGH);
  analogWrite(RmotorPWM,90);
  analogWrite(LmotorPWM, 50);
}


void drive_forward(){
  digitalWrite(Rmotor,HIGH); 
  digitalWrite(Lmotor,HIGH);
  analogWrite(RmotorPWM,75);
  analogWrite(LmotorPWM, 75);
}
