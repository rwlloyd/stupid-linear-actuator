// Make a single stepper bounce from one soft limit to another
//
// Copyright (C) 2012 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

// Modified by R Lloyd. 2021.

#include <AccelStepper.h>
 /// https://www.pjrc.com/teensy/td_libs_AccelStepper.html

const int motorInterfaceType = 1; // Not strictly necessary, but helps explain the random 1 in the function call
const int stepPin = 0;            //D3
const int dirPin = 2;             //D4
const int enablePin = 4;          //D2
const int homePin = 5;            //D1

const float actuator_travel_mm = 88;    // mm
const int homing_speed = 2000;          // stepsPerSecond
const int homing_backoff = 50;          // steps    
const float stepsPerMM = 44.44;         // https://blog.prusaprinters.org/calculator_3416/
int actuator_travel_steps = actuator_travel_mm * stepsPerMM;

boolean homed = false;  // keep track of awareness of the 1D world
int position = 0;       // keep track of the cirrent position

// Define a stepper and the pins it will use
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

void setup() {
  pinMode(homePin, INPUT_PULLUP);
  
  Serial.begin(115200);
  delay(1000);
  // Change these to suit your stepper if you want this is for a4988, 3200 steps/rev
  stepper.setMaxSpeed(1000); //stepsPerSecond
  stepper.setAcceleration(1000); //stepsPerSecondSquared

  int distToHome = findHome();
  position = -distToHome;
  Serial.print(distToHome);
  Serial.println(" steps!");
}

void loop() {
  if (!homed){
    findHome();
  } else{
  //Serial.println(digitalRead(homePin));
  // If at the end of travel go to the other end
  // if (stepper.distanceToGo() == 0)
  //   stepper.moveTo(-stepper.currentPosition());
  Serial.println(position);
  stepper.run();
  }
}

int findHome() {
  // Function to move the carriage of the actuator towards the home switch and then 
  // record the distance and compare it to the known size of the actuator from initial setup
  int step_count = 0;
  if (!digitalRead(homePin)){
    stepper.move(20);
  }
  else
  {
    homed = true;
    Serial.print("homed!"); 
    homingBackoff();
  }
  stepper.run();
}

void homingBackoff(){
  stepper.setMaxSpeed(homing_speed);
  //stepper.moveTo(-position + homing_backoff);
  stepper.move(-homing_backoff);
}