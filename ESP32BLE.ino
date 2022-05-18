#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Declareer de verschillende schakelaars.
int S1 = 34; // Used
int S2 = 35; // Used
int S3 = 32; // Used
//int S4 = 33;
int S5 = 21;
//int S6 = 25;
int analogInputPinX = 2; // Used
int analogInputPinY = 15; // Used
int digitalInputPin = 18; // Used
int analogInputValX;
int analogInputValY;
int last_analogInputValX;
int last_analogInputValY;
int digitalInputVal;
int digitalOutputVal;

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
  Serial.begin(115200);
  pinMode (S1, INPUT);
  pinMode (S2, INPUT);
  pinMode (S3, INPUT);
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
    
    if (digitalRead(S1) == HIGH)
        {
        digitalOutputVal = 1;
        }
        // Stuur 2 wanneer schakelaar S2 aan pin 35 wordt ingedrukt.
        else if (digitalRead(S2) == HIGH)
        {
         digitalOutputVal = 2;
        }
        // Stuur 3 wanneer schakelaar S3 aan pin 32 wordt ingedrukt.
        else if (digitalRead(S3)== HIGH)
        {
         digitalOutputVal = 3;
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
 
