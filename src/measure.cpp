#include <Arduino.h>
#include "../lib/ADS1X15/ADS1X15.h"
#include "measure.h"
#include "debug.h"

//----------------------------------------------------------------------------------------------------------------------

#define FILTER_MAX_HISTORY 15
#define FILTER_MIN_AVERAGE 5
#define NUM_SAMPLES_PER_MEASUREMENT 8
#define RESISTOR 1

//----------------------------------------------------------------------------------------------------------------------

struct filtered_data latest_data;

float history[2][FILTER_MAX_HISTORY]; //[v/i]
uint8_t history_index;

ADS1115 ADS(0x48);

//----------------------------------------------------------------------------------------------------------------------

static void measure(float result[2]);

//----------------------------------------------------------------------------------------------------------------------
// PUBLIC
//----------------------------------------------------------------------------------------------------------------------

void measure_init ()
{
    ADS.begin();
    ADS.setGain(0);  // 6.144 volt
    ADS.setDataRate(7); // 860 mps
    filter_take_first_data();

    #if DEBUG_INIT
        Serial.println ("-- INIT: Measure");
    #endif
}

//----------------------------------------------------------------------------------------------------------------------

void filter_take_first_data(){
    float result[2];
    measure (result);

    for (uint8_t i = 0; i < FILTER_MAX_HISTORY; i++){
        history[0][i] = result[0];
        history[1][i] = result[1];
    }

    history_index = 0;
    latest_data.voltage = result[0];
    latest_data.voltage_der = 0;
    latest_data.current = result[1]/RESISTOR;
    latest_data.current_der = 0;
    latest_data.time = millis();
}

//----------------------------------------------------------------------------------------------------------------------

void filter_take_new_data() {
    float result[2];
    float ddt[2];

    LOG_TIME("Filter start");

    measure(result);

    LOG_TIME("Measure in filter end");

    latest_data.time = millis();

    history_index = (history_index + 1) % FILTER_MAX_HISTORY;

    history[0][history_index] = result[0];
    history[1][history_index] = result[1];

    /**
     * Если измерения делаются равномерно, то есть x_i = i*t, где i в [0, n], то МНК имеет вид
     * k = \sum{ (i-(n-1)/2)*y_i } / \sum{ (i-(n-1)/2)^2*t }
     */

    float sum_xy = 0;
    float sum_xx = 0;
    const int n_2 = (FILTER_MAX_HISTORY-1) / 2;
    float tmp;

    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < FILTER_MAX_HISTORY; ++j)
        {
            uint8_t hist_i = (history_index + j + 1)%FILTER_MAX_HISTORY;

            tmp = (j - n_2);
            sum_xy += history[i][hist_i] * tmp;
            sum_xx += tmp * tmp;
        }

        ddt[i] = sum_xy / sum_xx / MEASURE_MILLIS * 1000 * 1000;
    }

    #if DEBUG_DUMP_HISTORY
        static uint32_t dump_n = 0;
        Serial.print(history_index);
        Serial.print(", ");

        for (int i = 0; i < FILTER_MAX_HISTORY; ++i)
        {
            Serial.print(history[0][i], 4);
            Serial.print(", ");
        }
        Serial.print(dump_n);
        Serial.print(", ");
        Serial.println(ddt[0], 3);
        dump_n++;
    #endif

    LOG_TIME("Calc end in filter");

    latest_data.voltage = result[0];
    latest_data.current = result[1]/RESISTOR;

    latest_data.voltage_der = ddt[0];
    latest_data.current_der = ddt[1]/RESISTOR;
}

//----------------------------------------------------------------------------------------------------------------------
// STATIC
//----------------------------------------------------------------------------------------------------------------------

static void measure(float result[2])
{
    result[0] = result[1] = 0;
    int16_t val_01;
    int16_t val_23;

    for (int i = 0; i < NUM_SAMPLES_PER_MEASUREMENT; ++i) {
        val_01 = ADS.readADC_Differential_0_1();
        result[0] += ADS.toVoltage(val_01);

        val_23 = ADS.readADC_Differential_2_3();
        result[1] += ADS.toVoltage(val_23);
    }

    for (int i = 0; i < 2; ++i)
    {
        result[i] /= NUM_SAMPLES_PER_MEASUREMENT;
    }
}