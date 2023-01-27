#ifndef IPHO2012_REWORKED_LED_H
#define IPHO2012_REWORKED_LED_H

#include <inttypes.h>

void led_init();
[[noreturn]] void led_blink_signal (uint8_t pattern);

#endif //IPHO2012_REWORKED_LED_H
