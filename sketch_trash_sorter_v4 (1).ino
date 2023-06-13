// UCI Senior Project 2023 - Dustpan and Rail Controller

#include <Servo.h>
Servo myservo; // create servo object for dustpan retractor
int pos = 0; // variable to store the servo position, 0 to 180 degrees

# define D_MAG 11 // dustpan magnetic latch control, active high

bool cal = false; // calibration done if true
double X = 0; // current rail position
double NX = 0; // desired rail position
int MoveSpeed = 1000; // step delay in microseconds
char cmd = 'n';
String inputString = "help";

# define X_ENgnd  2 // ENA-(ENA) stepper motor enable- 
# define X_EN_5v  3 // ENA+(+5V) stepper motor enable+, active low
# define X_DIRgnd 4 // DIR-(DIR) stepper motor direction-
# define X_DIR_5v 5 // DIR+(+5v) stepper motor direction+, low up and high down (towards motor)
# define X_STPgnd 6 // PUL-(PUL) stepper motor step-
# define X_STP_5v 7 // PUL+(+5v) stepper motor step+, active high
# define X_ALM_5v 12 // ALM+(+5v) stepper motor alarm+, active low
# define X_BUTTON 13
double bin1 = 0; // recycle bin 1 stepper position
double bin2 = 10415; // recycle bin 2 stepper position
double bin3 = 20830; // recycle bin 3 stepper position
double bin4 = 31245; // recycle bin 4 stepper position
int x;

void setup() {

  Serial.begin(115200);
  //Serial.println("Dustpan and Rail Controller");

  pinMode (X_ENgnd ,OUTPUT); //ENA-(ENA)
  pinMode (X_EN_5v ,OUTPUT); //ENA+(+5V)
  pinMode (X_DIRgnd,OUTPUT); //DIR-(DIR)
  pinMode (X_DIR_5v,OUTPUT); //DIR+(+5v)
  pinMode (X_STPgnd,OUTPUT); //PUL-(PUL)
  pinMode (X_STP_5v,OUTPUT); //PUL+(+5v)
  pinMode (X_BUTTON,INPUT_PULLUP); //BUTTON
  pinMode (X_ALM_5v,INPUT_PULLUP); //ALM+(+5v)
  digitalWrite (X_ENgnd, LOW); //ENA-(ENA)
  digitalWrite (X_EN_5v, LOW); //ENA+(+5V), HIGH disabled and LOW enabled
  digitalWrite (X_DIRgnd, LOW); //DIR-(DIR)
  digitalWrite (X_DIR_5v, HIGH); //DIR+(+5v), HIGH toward motor and LOW away from motor
  digitalWrite (X_STPgnd, LOW); //PUL-(PUL)

  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  pinMode(D_MAG,OUTPUT);  // dustpan magnetic latch control
  digitalWrite(D_MAG, LOW);  // disable dustpan magnetic latch

  // Retract Dustpan
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees in one degree steps
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);                       // waits 25ms for the servo to reach the position
  }
  // Latch Dustpan
  digitalWrite(D_MAG, HIGH);
  // Loosen Dustpan
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees in one degree steps
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);                       // waits 25ms for the servo to reach the position
  }

  // Calibrate Rail Home
  while (cal == false) {
    //Serial.println("Calibrate Rail Home?");
    //Serial.print("Enter 'y' if yes, 'n' if no: ");
    //readSerialMon();
    if (cmd == 'y') {
      Serial.println("Press <Enter> once to start cal, then again when rail hits end");
      readSerialMon();
      while (Serial.available() == 0) {
        digitalWrite (X_STP_5v, HIGH);
        delayMicroseconds (MoveSpeed);
        digitalWrite (X_STP_5v, LOW);
        delayMicroseconds (MoveSpeed);
      }
      readSerialMon();
      //Serial.println("Stopped at rail end");
      digitalWrite (X_DIR_5v, LOW); // change direction to away from motor
      for (int i=0; i<500; i++) { 
        digitalWrite (X_STP_5v, HIGH);
        delayMicroseconds (MoveSpeed);
        digitalWrite (X_STP_5v, LOW);
        delayMicroseconds (MoveSpeed);
      }
      X = bin1;
      MoveSpeed=80;
      cal = true;
    }
    else if (cmd == 'n') {
      X = bin1;
      MoveSpeed=80;
      cal = true;
    }
    else {Serial.println("Unrecognizable input");}
  }
}  

void loop() {
  if(digitalRead(X_BUTTON) == LOW){
        Serial.print("test");
        //Serial.print("Enter recycle bin number (1, 2, 3, or 4): ");
        readSerialMon();
        
       
      switch (cmd) {
        case '1': NX = bin1;moveServo();break;    
        case '2': NX = bin2;moveRail();moveServo();break;    
        case '3': NX = bin3;moveRail();moveServo();break;    
        case '4': NX = bin4;moveRail();moveServo();break;    
        default: Serial.println("Unsupported bin");break;   
          
      }
      NX = bin1;
      moveRail(); 
  }// return to home
}

void readSerialMon() {
  while (Serial.available() == 0) {
  }
  inputString = Serial.readStringUntil("\n");
  cmd = inputString.charAt(0);

  //Serial.println(cmd);
}

void moveRail() { // starting position = X, ending position = NX
  int xt;
  if (NX>X)
    {digitalWrite (X_DIR_5v,LOW); xt=1;}
  else if (NX<X)
    {digitalWrite (X_DIR_5v,HIGH); xt=-1;}
  for (; X!=NX; X=X+xt) {
    digitalWrite (X_STP_5v, HIGH);
    delayMicroseconds (MoveSpeed);
    digitalWrite (X_STP_5v, LOW);
    delayMicroseconds (MoveSpeed);
  }
  X = NX;
}

void moveServo() {
  delay(500); 
  digitalWrite(D_MAG, LOW);
  delay(1000); 
  // Retract Dustpan
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees in one degree steps
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);                       // waits 15ms for the servo to reach the position
  }
  // Latch Dustpan
  digitalWrite(D_MAG, HIGH);
  // Loosen Dustpan
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees in one degree steps
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);                       // waits 15ms for the servo to reach the position
  }  
}
