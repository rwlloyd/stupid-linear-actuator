//Sketch to crudely control a stepper driven linear actuator over serialEvent
// 
// On start up, the actuator will home towards the expected endstop. After homing it will await 
// serial input via the console. 
// +ve numbers move x steps away from the endstop
// -ve numbers move x steps trowards the endstop 
// If you find the opposite of this true, flip the phases of the stepper motor.

// R Lloyd. 2021.

#include <AccelStepper.h>
// https://www.pjrc.com/teensy/td_libs_AccelStepper.html

const int motorInterfaceType = 1; // Not strictly necessary, but helps explain the random 1 in the function call
const int stepPin = 0; //D3
const int dirPin = 2; //D4
const int enablePin = 4; //D2
const int homePin = 5; //D1

const float actuator_travel_mm = 88; // mm
const int max_speed = 100000; //stepsPerSecond
const int max_acceleration = 10000; //stepsPerSecondSquared
const int homing_speed = 5000; // stepsPerSecond
const int homing_backoff = 100; // steps    
const float stepsPerMM = 44.44; // https://blog.prusaprinters.org/calculator_3416/
int actuator_travel_steps = actuator_travel_mm * stepsPerMM;

boolean homed = false; // keep track of awareness of the 1D world
int position = 0; // keep track of the cirrent position

bool enabled = false;

// Variables to recieve a new message byte by byte as a string of text
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

int newMessage;
bool commandRecieved = false;

// Define a stepper and the pins it will use
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

void setup() {
  pinMode(homePin, INPUT_PULLUP);
  pinMode(enablePin, OUTPUT);

  Serial.begin(115200);
  inputString.reserve(200);
  delay(1000);
  // Change these to suit your stepper if you want this is for a4988, 3200 steps/rev
  stepper.setMaxSpeed(max_speed); //stepsPerSecond
  stepper.setAcceleration(max_acceleration); //stepsPerSecondSquared

  //enabled = true;

}

void loop() {
  // Should we be enabled or Not
  //digitalWrite(enablePin, !enabled); // need to work out how to structure this?
  if (!homed) {
    //Serial.println(stepper.currentPosition());
    findHome();
  } 
  else {
    position = stepper.currentPosition();
    if (commandRecieved) {
      stepper.move(newMessage);
      commandRecieved = false;
    }
  //Serial.println(stepper.currentPosition());  
  }

  while (Serial.available() > 0) { // While therse still some information to read
    Serial.println("Something Definitely happenned");
    char inChar = (char)Serial.read(); //Get the new byte, read it as a char
    // add it to the inputString:
    inputString += inChar;
    if (inChar == '\n') { // If you find a newline
      newMessage = inputString.toInt();
      Serial.println(newMessage); // Parrot the message back  
      commandRecieved = true; // We have a message! It's dealt with in the main loop  
      inputString = ""; //Reset the input string, ready for the next message
    }
  }
  stepper.run();
}

int findHome() {
  // Function to move the carriage of the actuator towards the home switch and then 
  // record the distance and compare it to the known size of the actuator from initial setup
  int step_count = 0;
  bool touch_off = false;
  if (!digitalRead(homePin)) {
    Serial.println("Searching for home point");
    stepper.move(-20);
  } else {
    Serial.print("Position Before homing_backoff: ");
    Serial.println(stepper.currentPosition());
    stepper.move(homing_backoff);
    //Serial.println("homed, continue executing in the main loop");
    Serial.print("Position After Backoff: ");
    Serial.println(stepper.currentPosition());
    homed = true;
  }
  stepper.run();
}

// void serialEvent() {
//   while (Serial.available() > 0) { // While therse still some information to read
//     Serial.println("Something Definitely happenned");
//     char inChar = (char)Serial.read(); //Get the new byte, read it as a char
//     // add it to the inputString:
//     inputString += inChar;
//     if (inChar == '\n') { // If you find a newline
//       newMessage = inputString.toInt();
//       Serial.println(newMessage); // What was the message?  
//       commandRecieved = true; // We have a message! It's dealt with in the main loop  
//       inputString = ""; //Reset the input string, ready for the next message
//     }
//   }
// }
//Serial.println(digitalRead(homePin));
// If at the end of travel go to the other end
// if (stepper.distanceToGo() == 0)
//   stepper.moveTo(-stepper.currentPosition());