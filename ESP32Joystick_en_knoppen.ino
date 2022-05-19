#include <BluetoothSerial.h>
#include "esp_bt_main.h"
#include "esp_bt_device.h"

  // Declareer de verschillende schakelaars.
int B1 = 35; // Used
int Trigger1 = 34; // Used
int B2 = 33; // Used
int Trigger2 = 32; // Used
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

BluetoothSerial SerialBT;
byte BTData;

  
  // Initialiseer de Bluetooth (teruggave tekst op de monitor)
bool initBluetooth()
{
  if (!btStart()) {
    Serial.println("Failed to initialize controller");
    return false;
  }
 
  if (esp_bluedroid_init() != ESP_OK) {
    Serial.println("Failed to initialize bluedroid");
    return false;
  }
 
  if (esp_bluedroid_enable() != ESP_OK) {
    Serial.println("Failed to enable bluedroid");
    return false;
  }
 
}
  // Zoek het Bluetooth MAC Adres en geef deze weer op de seriele monitor.
void printDeviceAddress() {
 
  const uint8_t* point = esp_bt_dev_get_address();
 
  for (int i = 0; i < 6; i++) {
 
    char str[3];
 
    sprintf(str, "%02X", (int)point[i]);
    Serial.print(str);
 
    if (i < 5){
      Serial.print(":");
    }
 
  }
  
}

void setup() {
  
  Serial.begin(115200);
  SerialBT.begin();
  
  pinMode (B1, INPUT);
  pinMode (Trigger1, INPUT);
  pinMode (B2, INPUT);
  pinMode (Trigger2, INPUT);
  pinMode (S1, INPUT);
  pinMode (S2, INPUT);
  pinMode(analogInputPinX,   INPUT);
  pinMode(analogInputPinY,   INPUT);
  pinMode(digitalInputPin,   INPUT);
  initBluetooth();
  printDeviceAddress();

}
 
void loop() {
/*
 * Inkomende en uitgaande data wordt weergegeven op de seriele compoort, mogelijk dat dit problemen veroorzaakt met de schakelaren. Test de schakelaardataversturing zonder dit stuk code:
 */
  // Feed any data from bluetooth to Terminal.
  if (SerialBT.available())
  Serial.write(SerialBT.read());
  
// Feed all data from terminal to bluetooth
  if (Serial.available())
  SerialBT.write(Serial.read());



  analogInputValX = analogRead(analogInputPinX);
  analogInputValY = analogRead(analogInputPinY);
  digitalInputVal = digitalRead(digitalInputPin);
  if (digitalInputVal != 1)
      {
      Serial.print("YAY ! Digital is ");
      Serial.print(digitalInputVal);
      Serial.println(" "); // println, to end with a carriage return
      delay(100);
      } 
      /*       
  if (analogInputValX != last_analogInputValX)    // X-as uitlezing wanneer er verandering is, deze werkt niet goed.
      {
      Serial.print("Change X to ");
      Serial.print(analogInputValX);    
      Serial.println(" "); // println, to end with a carriage return
      last_analogInputValX = analogInputValX;
      delay(100);
      }
  if (analogInputValY != last_analogInputValY)    // Y-as uitlezing wanneer er verandering is, deze werkt niet goed.
      {
      Serial.print("Change Y to ");
      Serial.print(analogInputValY);    
      Serial.println(" "); // println, to end with a carriage return
      last_analogInputValY = analogInputValY;
      delay(100);
      }
      */


// Print X value:
  Serial.print("Change X to ");
  Serial.print(analogInputValX);    
  Serial.println(" "); // println, to end with a carriage return
  last_analogInputValX = analogInputValX;
  delay(500);
// Print Y value:
  Serial.print("Change Y to ");
  Serial.print(analogInputValY);    
  Serial.println(" "); // println, to end with a carriage return
  last_analogInputValY = analogInputValY;
  delay(500);

  
  // Stuur 1 wanneer schakelaar B1 aan pin 34 wordt ingedrukt.
  if (digitalRead(B1) == HIGH)
  {
  SerialBT.println("1");
  Serial.write("1");
  delay(100);
  }
  // Stuur 2 wanneer schakelaar Trigger1 aan pin 35 wordt ingedrukt.
  if (digitalRead(Trigger1) == HIGH)
  {
  SerialBT.println("2");
  Serial.write("2");
  delay(100);
  }
  // Stuur 3 wanneer schakelaar B2 aan pin 32 wordt ingedrukt.
  if (digitalRead(B2)== HIGH)
  {
  SerialBT.println("3");
  Serial.write("3");
  delay(100);
  }

  // Stuur 4 wanneer schakelaar Trigger2 aan pin 33 wordt ingedrukt.
  if (digitalRead(Trigger2)== HIGH)
  {
  SerialBT.println("4");
  Serial.write("4");
  delay(100);
  }

  // Stuur 5 wanneer schakelaar S1 aan pin 25 wordt ingedrukt.
  if (digitalRead(S1)== HIGH)
  {
  SerialBT.println("5");
  Serial.write("5");
  delay(100);
  }

  // Stuur 6 wanneer schakelaar S2 aan pin 26 wordt ingedrukt.
  if (digitalRead(S2)== HIGH)
  {
  SerialBT.println("6");
  Serial.write("6");
  delay(100);
  }
  
  // Stuur de data ingevoerd in de monitor via Bluetooth/geef ontvangen data weer op de monitor.
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }
  delay(100);
}
