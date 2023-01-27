#include <Arduino.h>
#include "measure.h"
#include "buttons.h"
#include "led.h"
#include "debug.h"

//----------------------------------------------------------------------------------------------------------------------

static void take_action();
static void start_new_set();

//----------------------------------------------------------------------------------------------------------------------

enum process
{
    GREETING,
    MEASURING,
    BROWSING
};

struct state {
    enum process current_process;
} current_state;

//----------------------------------------------------------------------------------------------------------------------
// PUBLIC
//----------------------------------------------------------------------------------------------------------------------


void setup()
{
    Serial.begin(115200);

    measure_init();
    led_init();
    buttons_init();

    current_state.current_process = GREETING;

    // load indexes
}

#if DEBUG_RUNAWAY_DETECT
    uint32_t next_delay = 0;
#endif

#if DEBUG_LOOP_TIMING
uint32_t prev_timing    = 0;
uint32_t current_timing = 0;
#endif

bool is_button_pressed = false;

void loop()
{
    if (IS_BUTTONS_PRESSED()) {
        if (!is_button_pressed) {
            take_action();
            is_button_pressed = true;
        }
    } else {
        is_button_pressed = false;
    }

    if (current_state.current_process == MEASURING)
    {
        volatile uint32_t time = millis();
        uint32_t delay = time - time % MEASURE_MILLIS + MEASURE_MILLIS;

        LOG_TIME ("Loop start");

#if DEBUG_RUNAWAY_DETECT
        if (time > next_delay) {
            Serial.println("--- --- --- --- --- --- --- ---"  "\n"
                           "--- --- SKIPPED MEASURE --- ---"  "\n"
                           "--- --- --- --- --- --- --- ---");
        }
        next_delay = delay + MEASURE_MILLIS;
#endif

        while (millis() < delay) {
            _delay_us(10);
        }

        filter_take_new_data();

        LOG_TIME("Measure end");

        Serial.println("\n\n\n\n\n\n\n");
        Serial.print("Set: ");
        Serial.println(latest_data.set);
        Serial.print("N: ");
        Serial.println(latest_data.set_index);
        Serial.print("V    : ");
        Serial.println(latest_data.voltage, 3);
        Serial.print("dV/dt: ");
        Serial.println(latest_data.voltage_der, 3);
        Serial.print("I    : ");
        Serial.println(latest_data.current, 3);
        Serial.print("dI/dt: ");
        Serial.println(latest_data.current_der, 3);

        LOG_TIME ("Print end");

#if DEBUG_LOOP_TIMING
        Serial.print ("Loop time: "); Serial.println(millis() - time);
#endif
    }
}


//----------------------------------------------------------------------------------------------------------------------
//STATIC
//----------------------------------------------------------------------------------------------------------------------

static void take_action()
{
    int action = check_buttons();

    switch (action)
    {
        case 'b': // Begin
            if (current_state.current_process != MEASURING) {
                current_state.current_process = MEASURING;

                start_new_set();
            }
        break;

        case 'e': // End
            if (current_state.current_process != GREETING) {
                current_state.current_process = GREETING;
                Serial.println("Welcome back. Again (c)");
            }
        break;
    }
}

//----------------------------------------------------------------------------------------------------------------------

static void start_new_set()
{
    latest_data.set++;
    latest_data.set_index = 0;

    filter_take_first_data();
}