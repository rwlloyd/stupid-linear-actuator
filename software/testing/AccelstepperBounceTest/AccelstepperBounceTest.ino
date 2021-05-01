
// Make a single stepper bounce from one soft limit to another
//
// Copyright (C) 2012 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

// Modified by R Lloyd. 2021.

#include <AccelStepper.h>
/// https://www.pjrc.com/teensy/td_libs_AccelStepper.html

const int stepPin = 0;
const int dirPin = 2;
const int enablePin = 2;
const int motorInterfaceType = 1;

// Define a stepper and the pins it will use
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

//float rotationSpeed = 0.1; // 0.08;//0.08; //rev/s
//int stepsPerRot = 3200;
//int rotarySpeed = 3*rotationSpeed * stepsPerRot;

void setup()
{  
  delay(1000);
  // Change these to suit your stepper if you want this is for a4988, 3200 steps/rev
  // stepper.setMinPulseWidth(10);
  stepper.setMaxSpeed(1000); //stepsPerSecond
  stepper.setAcceleration(200); //stepsPerSecondSquared
  stepper.moveTo(400);
}

void loop()
{
    // If at the end of travel go to the other end
    if (stepper.distanceToGo() == 0)
      stepper.moveTo(-stepper.currentPosition());

    stepper.run();
}
