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
#define PN532_IRQ   (2)  //pin 3 of the RJ12 17 (2)
#define PN532_RESET (3)  // pin 4 of the RJ12 9  (3)

int send_data[6];

// Or use this line for a breakout or shield with an I2C connection:
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);


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
int encoderPin;
int motorpin1;
int motorpin2;
long duration,cm;
int n = 0;
bool valid = false;
bool process_it = false;
String x = "";
int LED = 5;
byte newbyte = 0;
int first, second, third, fourth;

bool found_tag = false;
//int interruptPin = 10; //might not be necessary

void setup() {
  Serial.begin(115200);
  pinMode (MISO, OUTPUT);
  SPCR |= bit (SPE); //turn on SPI in slave mode
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LED, OUTPUT);
  pos = 0;
  //process_it = false;
  myservo.attach(9);
  //turn on the interrupt
  SPI.attachInterrupt();
  
  //RFID initialization
  nfc.begin();

 uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);

  // configure board to read RFID tags
  nfc.SAMConfig();
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

void RFID_reading(){
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  //the RFID tag we are using has UID of 4 bytes
  uint8_t uidLength;        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  bool found_a_tag = false;
  
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  
  if (success) {
    bool found_a_tag = true;
    //the correct order is 209, 21, 230, and 219
    //for the one with blue tag
    //19-->25
    //E0--> 224
    //71 -->113
    //7C --> 124
    Serial.println("Found a card!");
    Serial.print("UID Length: ");
    Serial.print(uidLength, DEC);
    Serial.println(" bytes");

    if (found_a_tag){
        for (uint8_t i=0; i < uidLength; i++) 
        {
          //Serial.print(" 0x");
          //Serial.println(uid[i], HEX);
          Serial.print("sending");
          Serial.println(uid[i],DEC);
          SPDR = uid[i];
        }
      found_a_tag = false;
    }
  }

       else
  {
    // PN532 probably timed out waiting for a card
    Serial.println("Timed out waiting for a card");
  }

  delay(500);
  SPDR = 0;
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

void RFID_sendData(){
  
  boolean detector;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID from tag
  
  //Orange post it --> indicate turning right
  uint8_t obj1[] = {0x79, 0xC4, 0x4D, 0x50}; 
  uint8_t obj2[] = {0x69, 0x12, 0xCF, 0x14}; //Yellow post it
  uint8_t obj3[] = {0x19, 0xE0, 0x71, 0x7C};
  uint8_t uidLength;

  Serial.println("Sensor ready!");

      detector = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
      
    
    if(detector){
      Serial.println("Found a tag!");
      Serial.println("This is .....");
      //1 means stop
      if(memcmp(obj1, uid, 4) == 0){
        Serial.println("object 1: stop");
        Serial.println("sending 1");
        SPDR = 1;
        delay(10);
      }
      // 2 means turn cw
      else if (memcmp(obj2, uid, 4) == 0){
        Serial.println("Object 2: turn left");
        Serial.println("sending 2");
        SPDR = 2;
        delay(10);
      }
      //3 means ccw
      else if (memcmp(obj3, uid, 4) == 0){
        Serial.println("Object 3");
        Serial.println("sending 3");
        SPDR = 3;
        delay(10);
      }
//      //3 means spin around
//      else if (memcmp(obj4, uid, 4) == 0){
//        Serial.println("Object 4");
//        SPDR = 3;
//        delay(500);
//      }
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
  


/*custom port switching functi
on */
void set_ports(){
  
    if (buf[0] == '2'){
    //right motor
      if ((buf[1] == 'L') && (buf[2] == 'M') ){
          encoderPin = A0;
          motorpin1 = 4;
          motorpin2 = 6;
          pinMode(motorpin1, OUTPUT);
          pinMode(motorpin2, OUTPUT);
          pinMode(encoderPin, INPUT);        
    }
    //right motor
      else if ((buf[1] == 'R') && (buf[2] == 'M')){
          encoderPin = A0;
          motorpin1 = 4;
          motorpin2 = 6;
          pinMode(motorpin1, OUTPUT);
          pinMode(motorpin2, OUTPUT);
          pinMode(encoderPin, INPUT);      
    }
    //ultrasonic
      else if (buf[1] == 'U'){
        echoPin = A0;
        trigPin = 4;
        pinMode(trigPin, OUTPUT);
        pinMode(echoPin, INPUT);
    }
    //infrared sensor
      else if (buf[1] == 'I'){
        infra = 4;
        pinMode(infra, INPUT);
      
  }

    //Line Sensors
      else if (buf[1] == 'L'){
        line = A0;
        pinMode(line, INPUT);
      
    }
  }
  
 
else if (buf[0] == '3'){
    //left motor
    if ((buf[1] == 'L') && (buf[2] == 'M') ){
      encoderPin = A1;
      motorpin1 = 2;
      motorpin2 = 3;
      pinMode(motorpin1, OUTPUT);
      pinMode(motorpin2, OUTPUT);
      pinMode(encoderPin, INPUT); 
    }
    //right motor
    else if ((buf[1] == 'R') && (buf[2] == 'M')){
      encoderPin = A1;
      motorpin1 = 2;
      motorpin2 = 3;
      pinMode(motorpin1, OUTPUT);
      pinMode(motorpin2, OUTPUT);
      pinMode(encoderPin, INPUT); 
    }
    //ultrasonic
    else if (buf[1] == 'U'){
        echoPin = A1;
        trigPin = 3;
        pinMode(trigPin, OUTPUT);
        pinMode(echoPin, INPUT);
        Serial.println("J3 has set up for ultrasonic");
        Serial.println("Distance Data can be read now");
    }
    //infrared sensor
    else if (buf[1] == 'I'){
        infra = 7;
        pinMode(infra, INPUT);
      
  }

    //Line Sensors
    else if (buf[1] == 'L'){
        line = A1;
        pinMode(line, INPUT);
      
    }
  }
  if (buf[0] == '4'){
    //left motor
    if ((buf[1] == 'L') && (buf[2] == 'M') ){
          encoderPin = A2;
          motorpin1 = 8;
          motorpin2 = 5; //pwm pins
          pinMode(motorpin1, OUTPUT);
          pinMode(motorpin2, OUTPUT);
          pinMode(encoderPin, INPUT);       
    }
    //right motor
    else if ((buf[1] == 'R') && (buf[2] == 'M')){
          encoderPin = A2;
          motorpin1 = 8;
          motorpin2 = 5; //pwm pins
          pinMode(motorpin1, OUTPUT);
          pinMode(motorpin2, OUTPUT);
          pinMode(encoderPin, INPUT);       
    }
    //ultrasonic
    else if (buf[1] == 'U'){
        echoPin = A2;
        trigPin = 5;
        pinMode(trigPin, OUTPUT);
        pinMode(echoPin, INPUT);
    }
    //infrared sensor
    else if (buf[1] == 'I'){
        infra = 8;
        pinMode(infra, INPUT);    
  }

    //Line Sensors
    else if (buf[1] == 'L'){
        line = A2;
        pinMode(line, INPUT);
      
    }
  }
  if (buf[0] == '8'){
    //ultrasonic
    if (buf[1] == 'U'){
        echoPin = A3;
        trigPin = 9;
        pinMode(trigPin, OUTPUT);
        pinMode(echoPin, INPUT);
    }
    //infrared sensor
    else if (buf[1] == 'I'){
        infra = 9;
        pinMode(infra, INPUT);
      
  }

    //Line Sensors
    else if (buf[1] == 'L'){
        line = A3;
        pinMode(infra, INPUT);
      
    }
  }
  else if (buf[0] == '9'){
    if (buf[1] == 'L'){
        line = A5;
        pinMode(line, INPUT);
  }
 }
}
void loop() {

//    send_distance();
    //servo_demo();
    RFID_sendData();
    //RFID_reading();
    

//clear the buffer when a command is executed
  
  if (process_it){
    pos = 0;
    process_it = false;
  }
}
