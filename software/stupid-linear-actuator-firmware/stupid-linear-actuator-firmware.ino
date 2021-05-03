/*
 * WebSocketServer_LEDcontrol.ino
 *
 *  Created on: 26.11.2015
 *
 * Sketch to crudely control a stepper driven linear actuator over serialEvent
 *
 * On start up, the actuator will home towards the expected endstop. After homing it will await 
 * serial input via the console. 
 * +ve numbers move x steps away from the endstop
 * -ve numbers move x steps trowards the endstop 
 * If you find the opposite of this true, flip the phases of the stepper motor.
 *
 * R Lloyd. 2021.
 */

// For the stepper motor and swith stuff

#include <AccelStepper.h>
// https://www.pjrc.com/teensy/td_libs_AccelStepper.html

const String wifiSSID = "oort";
const String wifiPassword = "heliopause";

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

// For the webserver and websocket stuff 

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>

#define LED_RED     16
#define LED_GREEN   14
#define LED_BLUE    12

ESP8266WiFiMulti WiFiMulti;

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            webSocket.sendTXT(num, "Connected");
        }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get Text: %s\n", num, payload);

            if(payload[0] == '#') {
                // we get RGB data

                // decode rgb data
                uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);

                analogWrite(LED_RED,    ((rgb >> 16) & 0xFF));
                analogWrite(LED_GREEN,  ((rgb >> 8) & 0xFF));
                analogWrite(LED_BLUE,   ((rgb >> 0) & 0xFF));
            }

            break;
    }

}

void setup() {
    // Set up the hardware pins before i forget
    pinMode(homePin, INPUT_PULLUP);
    pinMode(enablePin, OUTPUT);

    // Reserve some memory for the input string over serial
    inputString.reserve(32);

    // Change these to suit your stepper if you want this is for a4988, 3200 steps/rev
    stepper.setMaxSpeed(max_speed); //stepsPerSecond
    stepper.setAcceleration(max_acceleration); //stepsPerSecondSquared

    //enabled = true;

    // Start up the serial interface because.... debugging
    //Serial.begin(921600);
    Serial.begin(115200);

    Serial.setDebugOutput(true);

    Serial.println();
    Serial.println();
    Serial.println();

    // Wait for 4 seconds for the wifiand all the stuff
    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    // Example stuff remove when necessary.
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);

    digitalWrite(LED_RED, 1);
    digitalWrite(LED_GREEN, 1);
    digitalWrite(LED_BLUE, 1);

    // WiFiMulti.addAP("oort", "heliopause");
    WiFiMulti.addAP("wifiSSID", "wifiPassword");

    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    // start webSocket server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    // yes, whaterver MDNS magic does, lets start that too
    if(MDNS.begin("esp8266")) {
        Serial.println("MDNS responder started");
    }

    // handle index
    server.on("/", []() {
        //send index.html
        server.send(200, 
        "text/html", 
        "<html><head><script>var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);connection.onopen = function () {  connection.send('Connect ' + new Date()); }; connection.onerror = function (error) {    console.log('WebSocket Error ', error);};connection.onmessage = function (e) {  console.log('Server: ', e.data);};function sendRGB() {  var r = parseInt(document.getElementById('r').value).toString(16);  var g = parseInt(document.getElementById('g').value).toString(16);  var b = parseInt(document.getElementById('b').value).toString(16);  if(r.length < 2) { r = '0' + r; }   if(g.length < 2) { g = '0' + g; }   if(b.length < 2) { b = '0' + b; }   var rgb = '#'+r+g+b;    console.log('RGB: ' + rgb); connection.send(rgb); }</script></head><body>LED Control:<br/><br/>R: <input id=\"r\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/>G: <input id=\"g\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/>B: <input id=\"b\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/></body></html>");
    });

    server.begin();

    // Add service to MDNS
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);

    digitalWrite(LED_RED, 0);
    digitalWrite(LED_GREEN, 0);
    digitalWrite(LED_BLUE, 0);

}

void loop() {
    // Websocket Stuff
    webSocket.loop();
    server.handleClient();

    // Stepper Motor Stuff
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

void findHome() {
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