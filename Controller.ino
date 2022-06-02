#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino_GFX_Library.h>
#include <U8g2lib.h>

// Definieer pins voor de Display.  
#define TFT_SCK    18 // Klopt
#define TFT_MOSI   23 // Klopt
#define TFT_MISO   19 // Klopt
#define TFT_CS     22 // Klopt
#define TFT_DC     21 // Klopt
#define TFT_RESET  17 // Klopt
int LED = 16;         // Klopt
// Definieer pins voor de schakelaars en joysticks.
int BumperL = 35;     // Klopt | BL = Bumper left
int TriggerL = 34;    // Klopt | TL = Trigger left
int BumperR = 33;     // Klopt
int TriggerR = 32;    // Klopt
int S1 = 25;          // Klopt | (Rechter knop)
int S2 = 26;          // Klopt | (Linker knop)
int digitalInputPin1 = 5;   // Klopt | Rechter joystick (+) | SW
int analogInputPinX1 = 2;   // Klopt | Rechter joystick (+) | VRx
int analogInputPinY1 = 15;  // Klopt | Rechter joystick (+) | VRy
int analogInputPinX2 = 4;   // Klopt | Linker joystick (T) | VRx
long analogInputValX1;      // Variabele voor Joystick 1
long analogInputValY1;      // Variabele voor Joystick 1
long last_analogInputValX1; // Variabele voor Joystick 1
long last_analogInputValY1; // Variabele voor Joystick 1
int digitalInputVal;        // Variabele voor Joystick 1
long analogInputValX2;      // Variabele voor Joystick 2
long last_analogInputValX2; // Variabele voor Joystick 2

bool SingleUse = 0;          // Variabele die het printen van activatie maar 1 keer uitvoert
int digitalOutputVal = 0;
BLEServer *pServer = NULL;
BLECharacteristic * connCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
 
#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

Arduino_ESP32SPI bus = Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO);
Arduino_ILI9341 display = Arduino_ILI9341(&bus, TFT_RESET);
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("Connected");
      deviceConnected = true;
    };
    void onDisconnect(BLEServer* pServer) {
      Serial.println("Disconnected");
      deviceConnected = false;
    }
};
void refreshDisplay() {
   display.setCursor(20,20);
   display.fillScreen(WHITE);
   delay(10);
   display.print("Mode:" + String(digitalOutputVal)); 
   display.setCursor(20,40);
   display.print("Y waarde:" + String(analogInputValY1));
   display.setCursor(20,60);
   display.print("X waarde:" + String(analogInputValX1));
}

 
class MyCallbacks: public BLECharacteristicCallbacks {
  
    void onWrite(BLECharacteristic *characteristic) {
      std::string value = characteristic->getValue();
      if (value.length() > 0) {
        Serial.print("Received Value: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]);
        Serial.println();
      }
    }

    void onRead(BLECharacteristic *characteristic) {
      std::string value = characteristic->getValue();

      if (value.length() > 0) {
        Serial.print("Writing: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]);
        Serial.println();
      }
    }
};

void setup() {
  pinMode (LED, OUTPUT);
 // Arduino_ESP32SPI bus = Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO);     // Maak een data bus van klasse Arduino ESP32SPI.
  //display = Arduino_ILI9341(&bus, TFT_RESET);     // Maak een object in de aangemaakte klasse.
  
  display.begin();              // Initialiseer de display.
  display.fillScreen(WHITE);    // Zet de achtergrond kleur van de display.
  display.setCursor(20, 20);    // Plaats de cursor op zijn beginstand om vanaf daar te schrijven (lezend in pixels).
  display.setTextSize(2);       // Stel de grootte van de tekst in (The value 1 (default) corresponds to 6×8 pixels, 2 to 12×16, 3 to 18×24, and so on).
  display.setTextColor(BLUE);   // Stel de kleur van de tekst in (andere kleuren staan in de library #include <U8g2lib.h> of zie deze pagina: https://techtutorialsx.com/2021/01/31/esp32-ili9341-display-hello-world/ ).
  digitalWrite(LED, HIGH);      // Zet het display scherm aan door de pin aan LED hoog te zetten.
  
  Serial.begin(115200);
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
  
  // Create the BLE Device
  BLEDevice::init("ESP32 Controller");
 
  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
 
  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);
 
  // Create a BLE Characteristic
  connCharacteristic = pService->createCharacteristic(
                                            CHARACTERISTIC_UUID,
                                            BLECharacteristic::PROPERTY_NOTIFY |
                                            BLECharacteristic::PROPERTY_READ | 
                                            BLECharacteristic::PROPERTY_WRITE
                                        );
  connCharacteristic ->addDescriptor(new BLE2902());
  connCharacteristic ->setCallbacks(new MyCallbacks());
  pService->start();
 
  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop(){


    analogInputValX1 = analogRead(analogInputPinX1);
    analogInputValY1 = analogRead(analogInputPinY1);
    digitalInputVal = digitalRead(digitalInputPin1);
    analogInputValX2 = analogRead(analogInputPinX2);

    // Stuur 1 wanneer schakelaar BumperL aan pin 35 wordt ingedrukt.
    if (digitalRead(BumperL) == HIGH)
        {
        digitalOutputVal = 1;
        connCharacteristic->setValue(std::to_string(digitalOutputVal)+"\n");
        connCharacteristic->notify();
        refreshDisplay();
        }
        
    // Stuur 2 wanneer schakelaar TriggerL aan pin 34 wordt ingedrukt.
    if (digitalRead(TriggerL) == HIGH)
        {
         digitalOutputVal = 2;
         connCharacteristic->setValue(std::to_string(digitalOutputVal)+"\n");
         connCharacteristic->notify();
        refreshDisplay();

        }
        
    // Stuur 3 wanneer schakelaar BumperR aan pin 33 wordt ingedrukt.
    if (digitalRead(BumperR)== HIGH)
        {
         digitalOutputVal = 3;
         connCharacteristic->setValue(std::to_string(digitalOutputVal)+"\n");
         connCharacteristic->notify();
         refreshDisplay();

        }
        
    // Stuur 4 wanneer schakelaar TriggerR aan pin 32 wordt ingedrukt.
    if (digitalRead(TriggerR)== HIGH)
        {
         digitalOutputVal = 4;
         connCharacteristic->setValue(std::to_string(digitalOutputVal)+"\n");
         connCharacteristic->notify();
         refreshDisplay();
        }
        
    // Stuur 5 wanneer schakelaar S1 aan pin 25 wordt ingedrukt.
    if (digitalRead(S1)== HIGH)
        {
         digitalOutputVal = 5;
         connCharacteristic->setValue(std::to_string(digitalOutputVal)+"\n");
         connCharacteristic->notify();
         refreshDisplay();
        }
        
    // Stuur 6 wanneer schakelaar S2 aan pin 26 wordt ingedrukt.
    if (digitalRead(S2)== HIGH)
        {
         digitalOutputVal = 6;
         connCharacteristic->setValue(std::to_string(digitalOutputVal)+"\n");
         connCharacteristic->notify();
         refreshDisplay();

        }

    // Stuur 7 wanneer schakelaar digitalInputVal aan pin 5 wordt ingedrukt.
    if (digitalRead(digitalInputVal)== LOW)
        {
         digitalOutputVal = 7;
         connCharacteristic->setValue(std::to_string(digitalOutputVal)+"\n");
         connCharacteristic->notify();
         refreshDisplay();        
        }
          
    // Lees de Y-waarde van Joystick 1 (+) uit en stuur deze via bluetooth naar de robot.
    if(last_analogInputValY1+20 != analogInputValY1 || last_analogInputValY1-20 != analogInputValY1 )
      {
        connCharacteristic->setValue(std::to_string(analogInputValY1)+"\n");
        connCharacteristic->notify(); 
        last_analogInputValY1 = analogInputValY1;   // Reset de laatste waarde door deze te overschrijven met de nieuwe waarde.

      }

    // Lees de X-waarde van Joystick 1 (+) uit en stuur deze via bluetooth naar de robot.
    if(last_analogInputValX1+20 != analogInputValX1 || last_analogInputValX1-20 != analogInputValX1 )
    {
        connCharacteristic->setValue(std::to_string(analogInputValX1)+"\n");
        connCharacteristic->notify(); 
        last_analogInputValX1 = analogInputValX1;   // Reset de laatste waarde door deze te overschrijven met de nieuwe waarde.
    }

      // Lees de X-waarde van Joystick 2 (T) uit en stuur deze via bluetooth naar de robot.
    if(last_analogInputValX2+20 != analogInputValX2 || last_analogInputValX2-20 != analogInputValX2 )
    {
        connCharacteristic->setValue(std::to_string(analogInputValX2)+"\n");
        connCharacteristic->notify(); 
        last_analogInputValX2 = analogInputValX2;   // Reset de laatste waarde door deze te overschrijven met de nieuwe waarde.
    }

    if (deviceConnected && (SingleUse != 1))
    {
        connCharacteristic->setValue("Hello from ESP32\n");
        connCharacteristic->notify();
        SingleUse = 1;    // Variabele die dit maar 1 keer laat uitvoeren.
    }
    
    // disconnecting
    if (!deviceConnected && oldDeviceConnected)
    {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected)
    {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
