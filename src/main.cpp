#include <Arduino.h>
#include "measure.h"
#include "debug.h"

void setup()
{
    Serial.begin(115200);

    measure_init();
}

#if DEBUG_RUNAWAY_DETECT
    uint32_t next_delay = 0;
#endif

#if DEBUG_LOOP_TIMING
uint32_t prev_timing    = 0;
uint32_t current_timing = 0;
#endif

void loop()
{
    volatile uint32_t time = millis();
    uint32_t delay = time - time%MEASURE_MILLIS + MEASURE_MILLIS;

    LOG_TIME ("Loop start");

    #if DEBUG_RUNAWAY_DETECT
        if (time > next_delay)
        {
            Serial.println ("--- --- --- --- --- --- --- ---"  "\n"
                            "--- --- SKIPPED MEASURE --- ---"  "\n"
                            "--- --- --- --- --- --- --- ---");
        }
        next_delay = delay + MEASURE_MILLIS;
    #endif

    while(millis() < delay) {
        _delay_us(10);
    }

    filter_take_new_data ();

    LOG_TIME("Measure end");

    Serial.println("\n\n\n\n\n\n\n");
    Serial.print ("V    : "); Serial.println (latest_data.voltage,     3);
    Serial.print ("dV/dt: "); Serial.println (latest_data.voltage_der, 3);
//    Serial.print ("I    : "); Serial.println (latest_data.current,     3);
//    Serial.print ("dI/dt: "); Serial.println (latest_data.current_der, 3);

    LOG_TIME ("Print end");

    #if DEBUG_LOOP_TIMING
        Serial.print ("Loop time: "); Serial.println(millis() - time);
    #endif
}