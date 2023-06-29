#include <SPI.h>
#include "../lib/SST25VF/SST25VF.h"

#include "debug.h"
#include "led.h"
#include "errors.h"
#include "measure.h"
#include "memory.h"
#include "config.h"

//----------------------------------------------------------------------------------------------------------------------

#if DEBUG_RECHECK_WRITE
    #include "../lib/LiquidCrystal_I2C-1.1.2/LiquidCrystal_I2C.h"
    extern LiquidCrystal_I2C lcd;
#endif

SST25VF mem;
uint32_t write_pos;

const int SAMPLES_OFFSET = 4096;

struct memory_header
{
    uint32_t total_measures;
    uint32_t last_set;
} memoryHeader;

//----------------------------------------------------------------------------------------------------------------------

void memory_init()
{
    mem.begin(FLASH_CE_PIN, FLASH_WP_PIN, FLASH_HOLD_PIN);

#if DEBUG_ERASE_FLASH
    mem.totalErase();
    mem.sectorErase(0);
#endif

    mem.readArray(0, (uint8_t *) &memoryHeader, sizeof(memoryHeader));

#if DEBUG_LOG_MEDEBUG_RECHECK_WRITEMORY_HEADER
    Serial.print("Memory header data: ");
    Serial.print(memoryHeader.total_measures);
    Serial.println(memoryHeader.last_set);
#endif

    latest_data.set       = memoryHeader.last_set;
    latest_data.abs_index = memoryHeader.total_measures;

#if DEBUG_INIT
    Serial.println ("-- INIT: Memory");
#endif
}

void memory_prepare_sampling ()
{
    write_pos = SAMPLES_OFFSET + memoryHeader.total_measures * sizeof (filtered_data);
}

void memory_sample ()
{
    mem.writeArray(write_pos, (const uint8_t *) &latest_data, sizeof (filtered_data));

#if DEBUG_RECHECK_WRITE
    filtered_data tmp;
    mem.readArray(write_pos, (uint8_t *) &tmp, sizeof(filtered_data));
    if (memcmp(&tmp, &latest_data, sizeof(filtered_data)) != 0) {
        Serial.println("MISMATCHED SAMPLE WRITE");
        lcd.clear();
        lcd.println("MISMATCHED SAMPLE WRITE");
    }
#endif

    write_pos += sizeof (filtered_data);

    latest_data.set_index++;
    latest_data.abs_index++;
}

void memory_fetch (filtered_data *buf, uint32_t abs_pos)
{
#if DEBUG_LOGICAL_MEM_FETCH_POS
    Serial.print ("Fetching at logical pos ");
    Serial.println (abs_pos);
#endif

    uint32_t pos = SAMPLES_OFFSET + abs_pos * sizeof (filtered_data);
    mem.readArray(pos, (uint8_t *) buf, sizeof(filtered_data));

#if DEBUG_LOGICAL_MEM_FETCH_POS
    Serial.print ("Fetched logical pos:");
    Serial.print (buf->abs_index);
#endif
}

void memory_stop_sampling ()
{
    memoryHeader.total_measures = latest_data.abs_index;
    memoryHeader.last_set       = latest_data.set;

#if DEBUG_LOG_MEMORY_HEADER
    Serial.print("Memory header data: ");
    Serial.print(memoryHeader.total_measures);
    Serial.print(" ");
    Serial.println(memoryHeader.last_set);
#endif

    mem.sectorErase(0);
    mem.writeArray(0, (const uint8_t *) &memoryHeader, sizeof(memory_header));

#if DEBUG_RECHECK_WRITE
    memory_header mem_header_check;
    mem.readArray(0, (uint8_t *) &mem_header_check, sizeof(memory_header));
    if (mem_header_check.total_measures != memoryHeader.total_measures) {
        Serial.print("MISMATCHED HEADER WRITE. Expected & Real total measures: ");
        lcd.clear();
        lcd.println("MISMATCHED HEADER WRITE");
        Serial.print(memoryHeader.total_measures);
        Serial.print(" ");
        Serial.println(mem_header_check.total_measures);
    }
#endif
}