#ifndef IPHO2012_REWORKED_MEASURE_H
#define IPHO2012_REWORKED_MEASURE_H

#include <inttypes.h>

struct __attribute__ ((__packed__)) filtered_data{
    uint16_t set;
    uint32_t abs_index;
    uint16_t set_index;
    uint32_t time;
    float current;
    float voltage;
    float current_der;
    float voltage_der;
}__attribute__((packed));

extern struct filtered_data latest_data;

void measure_init ();
void filter_take_new_data();
void filter_take_first_data ();

#endif //IPHO2012_REWORKED_MEASURE_H
