#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino_GFX_Library.h>
#include <U8g2lib.h>

//Devine 
#define TFT_SCK    18
#define TFT_MOSI   23
#define TFT_MISO   19
#define TFT_CS     22
#define TFT_DC     21
#define TFT_RESET  17
int LED = 16;
// Declareer de verschillende schakelaars.
int BumperL = 35; // Used | BL = Bumper left
int TriggerL = 34; // Used | TL = Trigger left
int BumperR = 33; // Used
int TriggerR = 32; // Used
int S1 = 25; // Used
int S2 = 26; // Used
int analogInputPinX = 2; // Used
int analogInputPinY = 15; // Used
int digitalInputPin = 5; // Used
long analogInputValX;
long analogInputValY;
long last_analogInputValX;
long last_analogInputValY;
int digitalInputVal;

BLEServer *pServer = NULL;
BLECharacteristic * connCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
 
#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
 
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
  Arduino_ESP32SPI bus = Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO);     // Maak een data bus van klasse Arduino ESP32SPI.
  Arduino_ILI9341 display = Arduino_ILI9341(&bus, TFT_RESET);     // Maak een object in de aangemaakte klasse.
  
  display.begin();              // Initialiseer de display.
  display.fillScreen(WHITE);    // Zet de achtergrond kleur van de display.
  display.setCursor(20, 20);    // Plaats de cursor op zijn beginstand om vanaf daar te schrijven (lezend in pixels).
  display.setTextSize(2);       // Stel de grootte van de tekst in (The value 1 (default) corresponds to 6×8 pixels, 2 to 12×16, 3 to 18×24, and so on).
  display.setTextColor(BLUE);   // Stel de kleur van de tekst in (andere kleuren staan in de library #include <U8g2lib.h> of zie deze pagina: https://techtutorialsx.com/2021/01/31/esp32-ili9341-display-hello-world/ ).
  display.print("I'm Alive!");  // Geef de tekst weer op de display.
  digitalWrite(LED, HIGH);
  
  Serial.begin(115200);
  pinMode (BumperL, INPUT);
  pinMode (TriggerL, INPUT);
  pinMode (BumperR, INPUT);
  pinMode (TriggerR, INPUT);
  pinMode (S1, INPUT);
  pinMode (S2, INPUT);
  pinMode(analogInputPinX,   INPUT);
  pinMode(analogInputPinY,   INPUT);
  pinMode(digitalInputPin,   INPUT);
  
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

void loop() {

    analogInputValX = analogRead(analogInputPinX);
    analogInputValY = analogRead(analogInputPinY);
    digitalInputVal = digitalRead(digitalInputPin);
    
    if (digitalRead(BumperL) == HIGH)
        {
        digitalOutputVal = 1;
        }
        // Stuur 2 wanneer schakelaar S2 aan pin 35 wordt ingedrukt.
        else if (digitalRead(TriggerL) == HIGH)
        {
         digitalOutputVal = 2;
        }
        // Stuur 3 wanneer schakelaar S3 aan pin 32 wordt ingedrukt.
        else if (digitalRead(BumperR)== HIGH)
        {
         digitalOutputVal = 3;
        }
        else if (digitalRead(TriggerR)== HIGH)
        {
         digitalOutputVal = 4;
        }
        else if (digitalRead(S1)== HIGH)
        {
         digitalOutputVal = 5;
        }
        else if (digitalRead(S2)== HIGH)
        {
         digitalOutputVal = 6;
        }
      connCharacteristic->setValue(std::to_string(digitalOutputVal)+"\n");
      connCharacteristic->notify();
    
    if(last_analogInputValY+5 != analogInputValY || last_analogInputValY-5 != analogInputValY )
    {
        connCharacteristic->setValue(std::to_string(analogInputValY)+"\n");
        connCharacteristic->notify(); 
    }
    if(last_analogInputValX+5 != analogInputValX ||last_analogInputValX-5 != analogInputValX )
    {
        
        connCharacteristic->setValue(std::to_string(analogInputValX)+"\n");
        connCharacteristic->notify(); 
    }
    if (deviceConnected) {
        connCharacteristic->setValue("Hello from ESP32\n");
        connCharacteristic->notify();

        delay(10000);
    }
 
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
 
