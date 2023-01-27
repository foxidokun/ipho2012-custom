#include <Arduino.h>
#include "measure.h"
#include "buttons.h"
#include "led.h"
#include "debug.h"
#include "memory.h"

//----------------------------------------------------------------------------------------------------------------------

#define SWITCH_SET_TIME_THRESHOLD_MS 700

//----------------------------------------------------------------------------------------------------------------------

static void take_action(int action);
static void start_new_set();
static void render (filtered_data *data);

//----------------------------------------------------------------------------------------------------------------------

enum process
{
    GREETING,
    MEASURING,
    BROWSING
};

struct state {
    enum process current_process;
    uint16_t browsing_index;
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
    memory_init();

    current_state.current_process = GREETING;

    // load indexes
}

#if DEBUG_RUNAWAY_DETECT
    uint32_t next_delay = 0;
#endif

#if DEBUG_LOOP_VERBOSE_TIMING
uint32_t prev_timing    = 0;
uint32_t current_timing = 0;
#endif

bool is_button_pressed      = false;
char last_pressed           = 0;
uint32_t first_pressed_time = 0;

void loop()
{
    if (IS_BUTTONS_PRESSED()) {
        int buttons = check_buttons();

        if (buttons == 'p' || buttons == 'n')
        {
            if (last_pressed == buttons) {
                if (millis() - first_pressed_time > SWITCH_SET_TIME_THRESHOLD_MS) {
                    take_action (toUpperCase(buttons));
                    last_pressed = 0;
                }
            } else {
                last_pressed = (char) buttons;
                first_pressed_time = millis();
            }
        }

        if (!is_button_pressed) {
            take_action (buttons);
            is_button_pressed = true;
        }
    } else {
        is_button_pressed = false;
        last_pressed = 0;
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

        render (&latest_data);

        LOG_TIME ("Print end");

#if DEBUG_LOOP_TOTAL_TIMING
        Serial.print ("Loop time: "); Serial.println(millis() - time);
#endif
    }
}


//----------------------------------------------------------------------------------------------------------------------
//STATIC
//----------------------------------------------------------------------------------------------------------------------

static void take_action(int action)
{
    filtered_data data;
    int set_tmp;

    switch (action)
    {
        case 'b': // Begin
            if (current_state.current_process != MEASURING) {
                current_state.current_process = MEASURING;

                start_new_set ();
            }
        break;

        case 'e': // End
            if (current_state.current_process != GREETING) {
                memory_stop_sampling();
                current_state.current_process = GREETING;
                Serial.println("Welcome back. Again (c)");
            }
        break;

        case 's':
            if (current_state.current_process == MEASURING)
            {
                memory_sample();
            }
        break;

        case 'p':
            if (current_state.current_process == GREETING)
            {
                current_state.browsing_index = latest_data.abs_index;
                current_state.current_process = BROWSING;
            }

            if (current_state.current_process == BROWSING) {
                if (current_state.browsing_index > 0) {
                    current_state.browsing_index--;
                }

                memory_fetch(&data, current_state.browsing_index);
                render(&data);
            }
            break;

        case 'n':
            if (current_state.current_process == BROWSING)
            {
                if (current_state.browsing_index + 1 < latest_data.abs_index)
                {
                    current_state.browsing_index++;
                }

                memory_fetch (&data, current_state.browsing_index);
                render (&data);
            }

            break;

        case 'P':
            if (current_state.current_process == BROWSING) {
                memory_fetch(&data, current_state.browsing_index);
                if (data.set > 1) {
                    current_state.browsing_index -= data.set_index + 1;
                }

                render(&data);
            }

            break;

        case 'N':
            if (current_state.current_process == BROWSING) {
                memory_fetch(&data, current_state.browsing_index);
                set_tmp = data.set;

                while (data.set == set_tmp && current_state.browsing_index + 1 < latest_data.abs_index) {
                    current_state.browsing_index++;
                    memory_fetch(&data, current_state.browsing_index);
                }

                render(&data);
            }

            break;
    }
}

//----------------------------------------------------------------------------------------------------------------------

static void start_new_set()
{
    latest_data.set++;
    latest_data.set_index = 0;

    memory_prepare_sampling();
    filter_take_first_data();
}

static void render (filtered_data *data)
{
    Serial.println("\n\n\n\n\n\n\n");
    Serial.print("Set: ");
    Serial.println(data->set);
    Serial.print("N: ");
    Serial.println(data->set_index);
    Serial.print("V    : ");
    Serial.println(data->voltage, 3);
    Serial.print("dV/dt: ");
    Serial.println(data->voltage_der, 3);
    Serial.print("I    : ");
    Serial.println(data->current, 3);
    Serial.print("dI/dt: ");
    Serial.println(data->current_der, 3);
}