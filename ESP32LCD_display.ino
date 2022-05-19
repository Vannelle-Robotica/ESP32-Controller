/*
 * Naam: Guido Annema
 * Project: ILI9341 Display aansturing (Hello World)
 * Versie: V1
 * Datum: 16-05-2022 J2P4
 * */

#include <Arduino_GFX_Library.h>

#define TFT_SCK    18
#define TFT_MOSI   23
#define TFT_MISO   19
#define TFT_CS     22
#define TFT_DC     21
#define TFT_RESET  17
int LED = 16;

void setup() {
  // put your setup code here, to run once:
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
}

void loop() {
  // put your main code here, to run repeatedly.
}
