#ifndef IPHO2012_REWORKED_DEBUG_H
#define IPHO2012_REWORKED_DEBUG_H

#define DEBUG_LOOP_VERBOSE_TIMING    0
#define DEBUG_LOOP_TOTAL_TIMING    0
#define DEBUG_RUNAWAY_DETECT 1
#define DEBUG_INIT           0
#define DEBUG_DUMP_HISTORY   0

#if DEBUG_LOOP_VERBOSE_TIMING
extern uint32_t prev_timing;
extern uint32_t current_timing;
    #define LOG_TIME(msg) \
    {                      \
        current_timing = millis(); \
                           \
        Serial.print(msg);                   \
        Serial.print(" -- "); Serial.print (current_timing); \
        Serial.print ("  (");                           \
        Serial.print (current_timing - prev_timing);    \
        Serial.println ("ms since last measure)");        \
        prev_timing = current_timing; \
        };
#else
    #define LOG_TIME(msg) {;};
#endif


#endif //IPHO2012_REWORKED_DEBUG_H
