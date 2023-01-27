#include "led.h"

#include <Arduino.h>
#include "debug.h"
#include "config.h"


void led_init()
{
    pinMode(LED_PIN, OUTPUT);

    #if DEBUG_INIT
        Serial.println ("-- INIT: Led");
    #endif
}

[[noreturn]] void led_blink_signal (const uint8_t pattern)
{
    while (true) {
        uint8_t pattern_copy = pattern;
        for (int i = 0; i < 8; ++i) {
            digitalWrite(LED_PIN, (pattern_copy & 0b1));
            pattern_copy = pattern_copy >> 1;

            delay (LED_INTERNAL_PAUSE_MS);
        }

        digitalWrite (LED_PIN, LOW);
        delay(LED_WORD_PAUSE_MS);
    }
}