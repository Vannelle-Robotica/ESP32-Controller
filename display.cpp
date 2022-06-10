#include "display.h"

namespace Controller {

    Display::Display(DisplayData &data) : data(data) {
        // Initialize display
        pinMode(LED, OUTPUT);
        digitalWrite(LED, HIGH);

        // Draw blank screen
        display.begin();
        display.fillScreen(WHITE);
        display.setCursor(20, 20);
        display.setTextSize(2);
        display.setTextColor(BLUE);
    }

    void Display::update() {
        // Check if data was updated since last update
        if (data == last) return;
        int line = 0;

        for (auto item: data) {
            // If item has never been printed
            if (last.find(item.first) == last.end()) {
                // Print field name
                display.setCursor(20, ++line * OFFSET_Y);
                display.print(item.first);

                // Print colon
                display.setCursor(VALUE_X - 10, line * OFFSET_Y + 2);
                display.print(":");

                // Print value if set
                display.setCursor(VALUE_X, line * OFFSET_Y);
                display.print(item.second);
                continue;
            }

            // Skip unchanged elements
            if (last[item.first] == item.second) {
                line++;
                continue;
            }

            // Write changed data to display
            display.fillRect(VALUE_X, ++line * OFFSET_Y, VALUE_X + 20, OFFSET_Y, WHITE);
            display.setCursor(VALUE_X, line * OFFSET_Y);
            display.print(item.second);
        }

        // Save last data
        last = data;
    }
}
