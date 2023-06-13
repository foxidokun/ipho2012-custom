#ifndef IPHO2012_REWORKED_DEBUG_H
#define IPHO2012_REWORKED_DEBUG_H

#define DEBUG_LOOP_VERBOSE_TIMING        0
#define DEBUG_LOOP_TOTAL_TIMING          0
#define DEBUG_RUNAWAY_DETECT             1
#define DEBUG_INIT                       0
#define DEBUG_DUMP_HISTORY               0
#define DEBUG_LOGICAL_MEM_FETCH_POS      0

#define DEBUG_ERASE_FLASH                0
#define DEBUG_LOG_MEMORY_HEADER          1
#define DEBUG_RECHECK_WRITE              1
#define DEBUG_MEM_CHECK_RETURN_VALUES    1
#define DEBUG_FLASH_ERROR_DETECTION      1



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
