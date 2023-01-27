#ifndef IPHO2012_REWORKED_BUTTONS_H
#define IPHO2012_REWORKED_BUTTONS_H

#include <inttypes.h>

int check_buttons ();
void buttons_init ();

extern uint8_t common_mask;
extern volatile uint8_t *buttons_port;

#define IS_BUTTONS_PRESSED() ((*buttons_port & common_mask) ^ common_mask)

#endif //IPHO2012_REWORKED_BUTTONS_H
