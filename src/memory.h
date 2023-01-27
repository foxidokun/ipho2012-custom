#ifndef IPHO2012_REWORKED_MEMORY_H
#define IPHO2012_REWORKED_MEMORY_H

void memory_init();
void memory_prepare_sampling ();
void memory_sample ();
void memory_stop_sampling ();
void memory_fetch (filtered_data *buf, uint32_t abs_pos);

#endif //IPHO2012_REWORKED_MEMORY_H
