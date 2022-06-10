#include "display.h"
#include "bluetooth.h"

using namespace Controller;

// Define pins for buttons.
int BumperL = 35;
int TriggerL = 34;
int BumperR = 33;
int TriggerR = 32;
int S1 = 25;          // Right button 
int S2 = 26;          // Left button
// Define pins for joysticks
int digitalInputPin1 = 5;   // Rechter joystick (+) | SW
int analogInputPinX1 = 2;   // Rechter joystick (+) | VRx
int analogInputPinY1 = 15;  // Rechter joystick (+) | VRy
int analogInputPinX2 = 4;   // Linker joystick (T) | VRx

long analogInputValX1;      // Variabele voor Joystick 1
long analogInputValY1;      // Variabele voor Joystick 1
long last_analogInputValX1; // Variabele voor Joystick 1
long last_analogInputValY1; // Variabele voor Joystick 1
int digitalInputVal;        // Variabele voor Joystick 1
long analogInputValX2;      // Variabele voor Joystick 2
long last_analogInputValX2; // Variabele voor Joystick 2
std::string directions = "";
std::string controllerOutput;
std::string lastControllerOutput;
int mappedx;
int digitalOutputVal = 0;


String modes[4] = { 
  "Autonomous",
  "Controlled",
  "LineDance",
  "Dancing"
};


bool modePressed;
int mode;


DisplayData displayData = {
        {"Button", ""},
        {"Direction", ""},
        {"Gewicht", ""},
        {"Mode", ""}
};


Display *display;

Bluetooth *bluetooth;

void sendControllerOutput() {
    controllerOutput = ("d " + directions + " b " + std::to_string(digitalOutputVal) + " s " + std::to_string(mappedx));
    if (controllerOutput != lastControllerOutput) {
        bluetooth->write(controllerOutput);
    }

    lastControllerOutput = controllerOutput;
}

void setup() {
    Serial.begin(115200);

    pinMode(BumperL, INPUT);
    pinMode(TriggerL, INPUT);
    pinMode(BumperR, INPUT);
    pinMode(TriggerR, INPUT);
    pinMode(S1, INPUT);
    pinMode(S2, INPUT);

    pinMode(digitalInputPin1, INPUT);
    pinMode(analogInputPinX1, INPUT);
    pinMode(analogInputPinY1, INPUT);
    pinMode(analogInputPinX2, INPUT);

    display = new Display(displayData);

    bluetooth = new Bluetooth("ESP32 Controller", onReceive);
}

void onReceive(const std::string &message) {
    displayData["Gewicht"] = String(message.c_str());
}

void loop() {
    analogInputValX1 = analogRead(analogInputPinX1);
    analogInputValY1 = analogRead(analogInputPinY1);
    
    digitalInputVal = digitalRead(digitalInputPin1);
    analogInputValX2 = analogRead(analogInputPinX2);

    // Stuur 1 wanneer schakelaar BumperL aan pin 35 wordt ingedrukt.
    if (digitalRead(BumperL) == HIGH) {
        digitalOutputVal = 1;
        displayData["Button"] = String(digitalOutputVal);
    }

    // Stuur 2 wanneer schakelaar TriggerL aan pin 34 wordt ingedrukt.
    if (digitalRead(TriggerL) == HIGH) {
        digitalOutputVal = 2;
        displayData["Button"] = String(digitalOutputVal);
    }

    // Stuur 3 wanneer schakelaar BumperR aan pin 33 wordt ingedrukt.
    if (digitalRead(BumperR) == HIGH) {
        digitalOutputVal = 3;
        displayData["Button"] = String(digitalOutputVal);
    }

    // Stuur 4 wanneer schakelaar TriggerR aan pin 32 wordt ingedrukt.
    if (digitalRead(TriggerR) == HIGH) {
        digitalOutputVal = 4;
        displayData["Button"] = String(digitalOutputVal);
    }

    // Stuur 5 wanneer schakelaar S1 aan pin 25 wordt ingedrukt.
    if (digitalRead(S1) == HIGH) {
        digitalOutputVal = 5;
        displayData["Button"] = String(digitalOutputVal);
    }

    // Stuur 6 wanneer schakelaar S2 aan pin 26 wordt ingedrukt.
    if (digitalRead(S2) == HIGH) {
        if(!modePressed) {
          if (++mode == 4)
              mode = 0;
          
          displayData["Mode"] = modes[mode];
          modePressed = true;
        }
    }else {
        modePressed = false;
    }

    if (analogInputValX1 >= 4000 && analogInputValY1 >= 0 && analogInputValY1 <= 4000) {
        directions = "f";
        displayData["Direction"] = "f";
    }
    
    if (analogInputValX1 <= 1500 && analogInputValY1 >= 0 && analogInputValY1 <= 4000) {
        directions = "b";
        displayData["Direction"] = "b";
    }
    
    if (analogInputValY1 >= 4000 && analogInputValX1 >= 0 && analogInputValX1 <= 4000) {
        directions = "rr";
        displayData["Direction"] = "rr";
    }
    
    if (analogInputValY1 <= 1800 && analogInputValX1 >= 0 && analogInputValX1 <= 4000) {
        directions = "rl";
        displayData["Direction"] = "rl";
    }
    
    if (analogInputValY1 >= 4000 && analogInputValX1 >= 4000 || analogInputValX1 == 0 && analogInputValY1 >= 4000) {
        directions = "tr";
        displayData["Direction"] = "tr";
    }

    if (analogInputValY1 == 0 && analogInputValX1 >= 4000 || analogInputValX1 == 0 && analogInputValY1 == 0) {
        directions = "tl";
        displayData["Direction"] = "tl";
    }
    
    if (analogInputValX1 > 0 && analogInputValX1 <= 4000 && analogInputValY1 > 0 && analogInputValY1 <= 4000) {
        directions = "s";
        displayData["Direction"] = "s";
    }

    // Lees de X-waarde van Joystick 2 (T) uit en stuur deze via bluetooth naar de robot.
    if (analogInputValX2 >= last_analogInputValX2 || analogInputValX2 <= last_analogInputValX2) {
        mappedx = map(analogInputValX2, 0, 4095, 0, 255);
        last_analogInputValX2 = analogInputValX2;
    }

    display->update();
    sendControllerOutput();
}
