#include "buttons.h"
#include "led.h"
#include "errors.h"
#include <Arduino.h>

#include "config.h"

uint8_t common_mask;
volatile uint8_t *buttons_port;

void buttons_init ()
{
    pinMode (BUTTON_START_BUTTON_PIN, INPUT_PULLUP);
    pinMode (BUTTON_STOP_BUTTON_PIN, INPUT_PULLUP);
    pinMode (BUTTON_PREV_BUTTON_PIN, INPUT_PULLUP);
    pinMode (BUTTON_NEXT_BUTTON_PIN, INPUT_PULLUP);
    pinMode (BUTTON_SAMPLE_BUTTON_PIN, INPUT_PULLUP);

    uint8_t common_port_check = digitalPinToPort(BUTTON_START_BUTTON_PIN);
    if (
            (digitalPinToPort(BUTTON_STOP_BUTTON_PIN)   != common_port_check) ||
            (digitalPinToPort(BUTTON_PREV_BUTTON_PIN)   != common_port_check) ||
            (digitalPinToPort(BUTTON_NEXT_BUTTON_PIN)   != common_port_check) ||
            (digitalPinToPort(BUTTON_SAMPLE_BUTTON_PIN) != common_port_check)
            )
    {
        led_blink_signal(ERRORS_BUTTONS_ON_DIFFERENT_PORTS);
    }

    buttons_port = portInputRegister(digitalPinToPort(BUTTON_START_BUTTON_PIN));
    common_mask  = digitalPinToBitMask(BUTTON_START_BUTTON_PIN) | digitalPinToBitMask(BUTTON_STOP_BUTTON_PIN) |
                   digitalPinToBitMask(BUTTON_PREV_BUTTON_PIN)  | digitalPinToBitMask(BUTTON_NEXT_BUTTON_PIN) |
                   digitalPinToBitMask(BUTTON_SAMPLE_BUTTON_PIN);

    #if DEBUG_INIT
        Serial.println ("-- INIT: Buttons");
    #endif
}

#define MATCH_BUTTON_TO_ACTION(pin, ch) {   \
if (digitalRead(pin) == LOW)                \
{                                           \
    return ch;                              \
}}

int check_buttons ()
{
    MATCH_BUTTON_TO_ACTION (BUTTON_START_BUTTON_PIN,  'b');
    MATCH_BUTTON_TO_ACTION (BUTTON_STOP_BUTTON_PIN,   'e');
    MATCH_BUTTON_TO_ACTION (BUTTON_PREV_BUTTON_PIN,   'p');
    MATCH_BUTTON_TO_ACTION (BUTTON_NEXT_BUTTON_PIN,   'n');
    MATCH_BUTTON_TO_ACTION (BUTTON_SAMPLE_BUTTON_PIN, 's');

    return -1;
}