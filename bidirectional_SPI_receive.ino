#include <Adafruit_PN532.h>
#include <SPI.h>
#include <Servo.h>
#include <Wire.h>

//Bi-directinal SPI 
//13 SCK  (serial clock)
//12 MISO (master in slave out)
//11 MOSI (master out slave in)
//10 SS (slave select)

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
//RFID module

Servo myservo;
//initialize the buffer
int bufSize = 4;
char buf [4];
volatile byte pos = 0;
//volatile boolean process_it;
//different sensors pins initialization
int trigPin = 3; //setting the pins for ultrasonic here for testing purpose
int echoPin = A1;
int infra;
int line;

//for setting up motor control
int motorpin1 = 2;
int motorpin2 = 3;
bool stop_motor = false;
bool cw = false;
bool ccw = false;

//distance sensor control
long duration,cm;
int n = 0;
bool valid = false;
bool process_it = false;
String x = "";
int LED = 5;
byte newbyte = 0;

//IR pin setup
int IRPin = 4;
int val;
//int interruptPin = 10; //might not be necessary

void setup() {
  Serial.begin(115200);
  pinMode (MISO, OUTPUT);
  SPCR |= bit (SPE); //turn on SPI in slave mode
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(IRPin, INPUT); 
  val = 0;
  pos = 0;
  //process_it = false;
  myservo.attach(9);
  //turn on the interrupt
  SPI.attachInterrupt();
  
}

//SPI ISR (Interrupt Service Routine)

ISR (SPI_STC_vect){
  
  byte c = SPDR; //get byte from the SPI data register
  //detect the beginning of the buffer, do not put it in the buffer
  if (c == '\n'){ 
    valid = true;
  }
  //detect the end character
  else if (c == '\r'){
    valid = false;
//    buf[0] = 0;
//    buf[1] = 0;
      pos = 0;
//  process_it = true;
  }
  //put data into the buffer
  if ((valid == true) && (c != '\n') && (c != '\r')){
    if (pos < bufSize ){  ///sizeof buffer
    buf [pos] = c;
    pos ++;
  }
  }
}


/*for servo demo 
 * make the servo to turn the angle specified by pi
*/
void servo_demo(){
  //send 'ss' with the start and end character
  //as indication of setting servo angles
    if (buf[0] == 's' && buf[1] == 's' ){
      int a = buf[2];
      Serial.println(a);
      myservo.write(a);
     
  }
}
  
void read_IR(){
  val = digitalRead(IRPin);
  SPDR = val;
  Serial.println(val);
}


/*Getting distance data from the ultrasonic sensors  */
void get_distance(){
  digitalWrite(trigPin,LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  pinMode(echoPin,INPUT);
  duration = pulseIn(echoPin,HIGH);
  cm = (duration/2)/29.1;

}

void check_buffer(){
    if (buf[0] == 's'){
      stop_motor = true;
      cw = false;
      ccw = false;
    }
    else if (buf[0] == 'c', buf[1] == 'w'){
      stop_motor = false;
      cw = true;
      ccw = false;
    }
    else if (buf[0] == 'c', buf[1] == 'c'){
      stop_motor = false;
      cw = false;
      ccw = true;
    }        
}

void motor_control(){
      check_buffer();
      while (stop_motor == true && cw == false && ccw == false){
        digitalWrite(motorpin2, LOW); //stop
        digitalWrite(motorpin1, LOW); 
        Serial.println("motor stop");
        check_buffer();
      }
 
//  
//  else if (buf[0] == 't' && buf[1] == 'l'){
//        digitalWrite(motor0pin2, LOW); //right motor, forward
//        digitalWrite(motor0pin1, HIGH); 
//        digitalWrite(motor1pin2, HIGH); //left motor, backward
//        digitalWrite(motor1pin1, LOW); 
//
//  }
//  //turn right commands
//  else if (buf[0] == 't' && buf[1] == 'r'){
//        digitalWrite(motor0pin2, HIGH);//right motor, backward
//        digitalWrite(motor0pin1, LOW);
//        digitalWrite(motor1pin2, LOW);//left motor, forward
//        digitalWrite(motor1pin1, HIGH); 
//  }
  //spin clockwise
    while (stop_motor == false && cw == true && ccw == false){
      digitalWrite(motorpin1, HIGH);
      digitalWrite(motorpin2, LOW);  
      Serial.println("clockwise");
      check_buffer();
    }


  //spin counterclockwise
    while (stop_motor == false && cw == false && ccw == true){
      digitalWrite(motorpin1, LOW);
      digitalWrite(motorpin2, HIGH);
      Serial.println("counterc-clockwise");
      check_buffer();
    }
  
}

/*for testing bidirectional communication
* in the blockly code. Taking in distance 
* data and make the bot do something in blockly
*/
void send_distance(){

  get_distance();
  
  if (buf[0] == 'd' && buf[1] == 'u'){
      SPDR = cm;
      Serial.println(cm);
  }
}
  

void loop() {

//    send_distance();
    //servo_demo();
    motor_control();
    

//clear the buffer when a command is executed
  
  if (process_it){
    pos = 0;
    process_it = false;
  }
}
