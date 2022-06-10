#include <Arduino_GFX_Library.h>
#include <U8g2lib.h>
//#include "display.h"
#include "bluetooth.h"

// Definieer pins voor de Display.  
#define TFT_SCK    18 // Klopt
#define TFT_MOSI   23 // Klopt
#define TFT_MISO   19 // Klopt
#define TFT_CS     22 // Klopt
#define TFT_DC     21 // Klopt
#define TFT_RESET  17 // Klopt
#define LED 16 
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
std::string directions= ""; 
String directionDisplay = "";
std::string controllerOutput;
std::string lastControllerOutput;
int mappedx;
String weight;
int digitalOutputVal = 0;
int modes = 0;

Arduino_ESP32SPI bus = Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO); // Maak een data bus van klasse Arduino ESP32SPI.
Arduino_ILI9341 display = Arduino_ILI9341(&bus, TFT_RESET); // Maak een object in de aangemaakte klasse.


void onReceive(const std::string &);

Bluetooth *bluetooth;

void sendControllerOutput() {
  controllerOutput = ("d " + directions + " b " + std::to_string(digitalOutputVal) + " s " + std::to_string(mappedx));
  if(controllerOutput != lastControllerOutput){
    bluetooth->write(controllerOutput);
  }
   
  lastControllerOutput = controllerOutput;
}

void refreshDisplay() {
   display.setCursor(20,20);
   display.fillScreen(WHITE);
   display.print("Button pushed: " + String(digitalOutputVal)); 
   display.setCursor(20,40);
   display.print("Y waarde: " + String(analogInputValY1));
   display.setCursor(20,60);
   display.print("X waarde: " + String(analogInputValX1));
   display.setCursor(20,80);
   int xy = sqrt((analogInputValY1)*2 + (analogInputValX1)*2);
   display.print("waarde: " + String(xy));
   display.setCursor(20,100);
   display.print("Direction: " + directionDisplay);
   display.setCursor(20,120);
   display.print("x waarde : " + String(mappedx));
   display.setCursor(20,140);
   display.print("Gewicht:  " + weight + "g");
   display.setCursor(20,160);
   display.print("Mode:  " + String(modes));
}

void setup() {
  
  Serial.begin(115200);
  pinMode (LED, OUTPUT);
  pinMode (BumperL, INPUT);
  pinMode (TriggerL, INPUT);
  pinMode (BumperR, INPUT);
  pinMode (TriggerR, INPUT);
  pinMode (S1, INPUT);
  pinMode (S2, INPUT);
  pinMode(digitalInputPin1,   INPUT);
  pinMode(analogInputPinX1,   INPUT);
  pinMode(analogInputPinY1,   INPUT);
  pinMode(analogInputPinX2,   INPUT);

  display.begin();              // Initialiseer de display.
  display.fillScreen(WHITE);    // Zet de achtergrond kleur van de display.
  display.setCursor(20, 20);    // Plaats de cursor op zijn beginstand om vanaf daar te schrijven (lezend in pixels).
  display.setTextSize(2);       // Stel de grootte van de tekst in (The value 1 (default) corresponds to 6×8 pixels, 2 to 12×16, 3 to 18×24, and so on).
  display.setTextColor(BLUE);   // Stel de kleur van de tekst in (andere kleuren staan in de library #include <U8g2lib.h> 
  digitalWrite(LED, HIGH);      // Zet het display scherm aan door de pin aan LED hoog te zetten.


  bluetooth = new Bluetooth("ESP32 Controller", onReceive);
}

void onReceive(const std::string &message) {
  weight = String(message.c_str());
  Serial.println(weight);
}

void loop(){
    analogInputValX1 = analogRead(analogInputPinX1);
    analogInputValY1 = analogRead(analogInputPinY1);
    digitalInputVal = digitalRead(digitalInputPin1);
    analogInputValX2 = analogRead(analogInputPinX2);
    bool updated;

    // Stuur 1 wanneer schakelaar BumperL aan pin 35 wordt ingedrukt.
    if (digitalRead(BumperL) == HIGH)
        {
        digitalOutputVal = 1;
        updated = true;
        }
        
    // Stuur 2 wanneer schakelaar TriggerL aan pin 34 wordt ingedrukt.
    if (digitalRead(TriggerL) == HIGH)
        {
         digitalOutputVal = 2;
        updated = true;
        }
        
    // Stuur 3 wanneer schakelaar BumperR aan pin 33 wordt ingedrukt.
    if (digitalRead(BumperR)== HIGH)
        {
         digitalOutputVal = 3;
        updated = true;
        }
        
    // Stuur 4 wanneer schakelaar TriggerR aan pin 32 wordt ingedrukt.
    if (digitalRead(TriggerR)== HIGH)
        {
         digitalOutputVal = 4;
        updated = true;
        }
        
    // Stuur 5 wanneer schakelaar S1 aan pin 25 wordt ingedrukt.
    if (digitalRead(S1)== HIGH)
        {
         digitalOutputVal = 5;
        updated = true;
        }
        
    // Stuur 6 wanneer schakelaar S2 aan pin 26 wordt ingedrukt.
    if (digitalRead(S2)== HIGH)
        {
         modes ++;
         if (modes == 4 ){
          modes = 0;
         }
         
        updated = true;
        }

    
    if(analogInputValX1 >= 4000 && analogInputValY1 >= 0 && analogInputValY1 <= 4000)
    {
      directions = "f"; 
      directionDisplay = "f";
    }
    if(analogInputValX1 <= 1500 && analogInputValY1 >= 0 && analogInputValY1 <= 4000)
    {
      directions = "b"; 
      directionDisplay = "b";
    }
    if(analogInputValY1 >= 4000 && analogInputValX1 >= 0 && analogInputValX1 <= 4000)
    {
      directions = "rr"; 
      directionDisplay = "rr";
    }
    if(analogInputValY1 <= 1800 && analogInputValX1 >= 0 && analogInputValX1 <= 4000)
    {
      directions = "rl"; 
      directionDisplay = "rl";
    }
    if(analogInputValY1 >= 4000 && analogInputValX1 >= 4000)
    {
      directions = "tr"; 
      directionDisplay = "tr";
    }
    if(analogInputValY1 == 0 && analogInputValX1 >= 4000)
    {
      directions = "tl"; 
      directionDisplay = "tl";
    }
    if(analogInputValX1 == 0 && analogInputValY1 >= 4000)
    {
      directions = "tr"; 
      directionDisplay = "tr";
    }
    if(analogInputValX1 == 0 && analogInputValY1 == 0)
    {
      directions = "tl"; 
      directionDisplay = "tl";
    }
    if(analogInputValX1 > 0 && analogInputValX1 <= 4000 && analogInputValY1 > 0 && analogInputValY1 <= 4000)
    {
      directions = "s"; 
      directionDisplay = "s";
    }
 

      // Lees de X-waarde van Joystick 2 (T) uit en stuur deze via bluetooth naar de robot.
    if(analogInputValX2 >= last_analogInputValX2 || analogInputValX2 <= last_analogInputValX2)
    {
        mappedx = map(analogInputValX2, 0, 4095, 0, 255);
        last_analogInputValX2 = analogInputValX2;   // Reset de laatste waarde door deze te overschrijven met de nieuwe waarde.
    }

    if(updated)
      refreshDisplay();
    sendControllerOutput();
}
