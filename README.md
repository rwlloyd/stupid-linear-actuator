# stupid-linear-actuator

Stupid (Not really smart) Linear actuator to help with moving things.

![Version1](./images/Version1.png)

STL files can be found in the STL folder. 

Initial version is based around 2020 aluminium extrusion. Other hardware:

    NEMA17 Stepper motor, 
    GT2-36-5mm Pulley, 
    Common microswitch 
    Appropriate length GT2 belt 
    2x M6x12, 
    4x M3x10
    2x ~M2.5 woodscrews 
    2x M3x30 or equivalent pins

# Electronics

Electronics are basically a copy of:

https://tasmota.github.io/docs/A4988-Stepper-Motor-Controller/

![a4988](./images/electronicsstartingpoint.jpg)

# Stepper Driver

A4988 Stepper Driver and carrier board

![a4988](./images/a4988jumperOptions.png) 

![a4983](./images/a4983.jpg)

Prototypes:
| A4988 Pin | MotorWire |
| --- | --- |
| 2B | Black |
| 2A | Green |
| 1A | Red |
| 2A | Blue |
    
A4983 is a version of the A4988 with onbord voltage regulator. Unfortunately it is discontinued and witll only be used on the prototypes for convenience. Future versions will require a buck converter.

# Buck converter

Not currently necessary  

# Homing Endstop

The addition of microswitch connected to the Weemos D1 Mini for homing is just a NC switch connected as a pulled up input to one of the wD1mini pins.

# System Overview

![System Overview](./images/SystemOverview.png)

## Setting up the Wemos D1 mini

Setup Instructions

    https://averagemaker.com/2018/03/wemos-d1-mini-setup.html

Install CH340 driver for wemos d1 mini usb serial communication

    https://www.wemos.cc/en/latest/ch340_driver.html

If the driver doesn't want to install select uninstall and then install again. seemed to work

## Moving the stepper motor.

I've worked with the Accelstepper library before. So let's use that:

http://www.airspayce.com/mikem/arduino/AccelStepper/

Install it into the arduino IDE in the usual way. Release notes would suggest that it just works, but lets see!

Yeah, this helps:

https://www.makerguides.com/a4988-stepper-motor-driver-arduino-tutorial/

Also this for the pin numbering on the wd1m:

https://chewett.co.uk/blog/1066/pin-numbering-for-wemos-d1-mini-esp8266/



# This bit is all new stuff to learn

A rest api for ESP8266
    https://github.com/marcoschwartz/aREST
    https://arest.io/

Separate Tutorial : REST server on esp8266 and esp32: introduction – Part 1
    https://www.mischianti.org/2020/05/16/how-to-create-a-rest-server-on-esp8266-and-esp32-startup-part-1/

ESP8266 HTTP server: Serving HTML, Javascript and CSS
    https://techtutorialsx.com/2016/10/15/esp8266-http-server-serving-html-javascript-and-css/

Voltage regulator and Motor driver
    https://www.pololu.com/product/1202 -- Got but discontinued