#include <Servo.h> //Elizabeth Garner as a part of Cornell Cup Minibot ECE 2020

Servo myservofing;  // create servo object to control the claw's fingers
Servo myservowrist;  // create servo object to control the claw's wrist
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
int fsrPin = A0;     // the FSR and 1K resistor are connected to a0
int fsrReading;     // the analog reading from the touch sensor
int n = 75;     // a max close angle in case the sensor is not triggered
int k = 350;    //temp variable for max sensor input

void setup() {
  myservofing.attach(9);  // sets finger servo on pin 9
  myservowrist.attach(8);  // sets wrist servo on pin 8
  Serial.begin(9600);   
}

void loop() {
 fsrReading = analogRead(fsrPin);  //initial analog touch sensor reading for testing
  Serial.print("Analog reading = ");
  Serial.println(fsrReading);
  
  Serial.println("Starting Demo!");
  Serial.println("Lowering Wrist");
  for (pos = 60; pos <= 90; pos += 1) { // wrist goes from 60 to 90 degrees
      // in steps of 1 degree
      myservowrist.write(pos);  // tell servo to go to position in variable 'pos'
      delay(30); // waits 30ms for the servo to reach the position
  }
  delay(1000);
  Serial.println("Opening Claw");
   for (pos = n; pos >= 10; pos -= 1) { // wrist goes from n (or 75) degrees to 10 degrees
    myservofing.write(pos);              // tell servo to go to position in variable 'pos'
    delay(30);                       // waits 30ms for the servo to reach the position
    fsrReading = analogRead(fsrPin);  //analog touch sensor reading
  }
  n = 75;     // reset max close angle in case the sensor is not triggered when closing
  delay(3000);
  Serial.println("Closing Claw");
  k = analogRead(fsrReading+50); //sets k value above current touch sensor reading
   for (pos = 10; pos <= n; pos += 1) { // finger goes from 10 to n (or 75) degrees
      // in steps of 1 degree
      fsrReading = analogRead(fsrPin);  //analog touch sensor reading
      Serial.print("Analog reading = ");
      Serial.println(fsrReading);
      if(fsrReading>(k+2)) { //if there is an increase in the touch sensor data by 2+
        n = pos; //n becomes the current position
        pos = n; //the loop ends 
      }else{ //if normal, the servo closes until it hits the max n value
      myservofing.write(pos);          // tell servo to go to position in variable 'pos'
      k = fsrReading; //'remembers' the current touch sensor reading to compare to again
      delay(50); // waits 50ms for the servo to reach pos
      }
   } delay(1000);
  Serial.println("Raising Wrist");
  for (pos = 90; pos >= 60; pos -= 1) { // wrist goes from 90 to 60 degrees
    myservowrist.write(pos);              // tell servo to go to position in variable 'pos'
    delay(30);                       // waits 30ms for the servo to reach the position
  }
  Serial.println("All Done!");
  k = 350;    // reset temp variable to determine increase in sensor data when claw is closing
  delay(3000);
  Serial.println("Putting Back Down!");
  Serial.println("Lowering Wrist");
  for (pos = 60; pos <= 90; pos += 1) { // wrist goes from 60 to 90 degrees
      // in steps of 1 degree
      myservowrist.write(pos);  // tell servo to go to position in variable 'pos'
      delay(30); // waits 30ms for the servo to reach the position
  }
  delay(1000);
  Serial.println("Opening Claw");
   for (pos = n; pos >= 10; pos -= 1) { // wrist goes from n (or 75) degrees to 10 degrees
    myservofing.write(pos);              // tell servo to go to position in variable 'pos'
    delay(30);                       // waits 30ms for the servo to reach the position
    fsrReading = analogRead(fsrPin);  //analog touch sensor reading
  }
  Serial.println("Raising Wrist");
  for (pos = 90; pos >= 60; pos -= 1) { // wrist goes from 90 to 60 degrees
    myservowrist.write(pos);              // tell servo to go to position in variable 'pos'
    delay(30);                       // waits 30ms for the servo to reach the position
  }
  Serial.println("Now It's All Done");
}
