#include "led.h"

#include <Arduino.h>
#include "debug.h"

#define LED_INTERNAL_PAUSE_MS 300
#define LED_WORD_PAUSE_MS 700

void led_init()
{
    pinMode(LED_BUILTIN, OUTPUT);

    #if DEBUG_INIT
        Serial.println ("-- INIT: Led");
    #endif
}

[[noreturn]] void led_blink_signal (const uint8_t pattern)
{
    while (true) {
        uint8_t pattern_copy = pattern;
        for (int i = 0; i < 8; ++i) {
            digitalWrite(LED_BUILTIN, (pattern_copy & 0b1));
            pattern_copy = pattern_copy >> 1;

            delay (LED_INTERNAL_PAUSE_MS);
        }

        digitalWrite (LED_BUILTIN, LOW);
        delay(LED_WORD_PAUSE_MS);
    }
}