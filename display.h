#ifndef DISPLAY_H
#define DISPLAY_H

#define TFT_SCK   18
#define TFT_MOSI  23
#define TFT_MISO  19
#define TFT_CS    22
#define TFT_DC    21
#define TFT_RESET 17
#define LED       16

#define VALUE_X   140
#define OFFSET_Y   20

#include <Arduino_GFX_Library.h>
#include <U8g2lib.h>

#include <map>

namespace Controller {

    // Type-alias used for storing display data
    using DisplayData = std::map<String, String>;

    class Display {
    public:
        Display(DisplayData &);

        // Updates the display if data was modified
        void update();

    private:
        Arduino_ESP32SPI bus = Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO);
        Arduino_ILI9341 display = Arduino_ILI9341(&bus, TFT_RESET);

        DisplayData &data;
        DisplayData last;
    };

}

#endif
