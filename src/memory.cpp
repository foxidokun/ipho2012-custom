#include <SPI.h>
#include "../lib/SPIMemory/src/SPIMemory.h"

#include "debug.h"
#include "led.h"
#include "errors.h"
#include "measure.h"
#include "memory.h"
#include "config.h"


const bool FAST_READ_ENABLED = !(DEBUG_FLASH_ERROR_DETECTION);
//TODO Прописать в вызовы функций

//----------------------------------------------------------------------------------------------------------------------

SPIFlash mem(FLASH_CE_PIN);
uint32_t write_pos;

struct memory_header
{
    uint32_t total_measures;
    uint32_t last_set;
} memoryHeader;

//----------------------------------------------------------------------------------------------------------------------

void memory_init()
{
    mem.begin(128);

#if DEBUG_ERASE_FLASH
    mem.totalErase();
#endif

    bool res = mem.readByteArray(0, (uint8_t *) &memoryHeader, sizeof(memoryHeader));

#if DEBUG_MEM_CHECK_RETURN_VALUES
    if (!res) {
        Serial.print("load header failed. Error code = ");
        Serial.println(mem.error());
    }
#endif

#if DEBUG_LOG_MEMORY_HEADER
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
    write_pos = sizeof (memory_header) + memoryHeader.total_measures * sizeof (filtered_data);
}

void memory_sample ()
{
    mem.writeByteArray(write_pos, (uint8_t *) &latest_data, sizeof (filtered_data));

#if DEBUG_RECHECK_WRITE
    filtered_data tmp;
    mem.readByteArray(write_pos, (uint8_t *) &tmp, sizeof(filtered_data));
    if (memcmp(&tmp, &latest_data, sizeof(filtered_data)) != 0) {
        Serial.println("MISMATCHED SAMPLE WRITE");
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

    uint32_t pos = sizeof (memory_header) + abs_pos * sizeof (filtered_data);
    bool res = mem.readByteArray(pos, (uint8_t *) buf, sizeof(filtered_data));

#if DEBUG_MEM_CHECK_RETURN_VALUES
    if (!res) {
        Serial.print("sample write failed. Error code = ");
        Serial.println(mem.error());
    }
#endif


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

    bool res = mem.writeByteArray(0, (uint8_t *) &memoryHeader, sizeof(memoryHeader));

#if DEBUG_MEM_CHECK_RETURN_VALUES
    if (!res) {
        Serial.print("mem header write failed. Error code = ");
        Serial.println(mem.error());
    }
#endif

#if DEBUG_RECHECK_WRITE
    memory_header mem_header_check;
    mem.readByteArray(0, (uint8_t *) &mem_header_check, sizeof(memory_header));
    if (mem_header_check.total_measures != memoryHeader.total_measures) {
        Serial.print("MISMATCHED HEADER WRITE. Expected & Real total measures: ");
        Serial.print(memoryHeader.total_measures);
        Serial.println(mem_header_check.total_measures);
    }
#endif
}