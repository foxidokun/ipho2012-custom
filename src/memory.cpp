#include <SPI.h>
#include "../lib/SD/src/SD.h"

#include "debug.h"
#include "led.h"
#include "errors.h"
#include "measure.h"
#include "memory.h"
#include "config.h"

//----------------------------------------------------------------------------------------------------------------------

File datafile;

struct memory_header
{
    uint32_t total_measures;
    uint32_t last_set;
} memoryHeader;

//----------------------------------------------------------------------------------------------------------------------

void memory_init()
{
    if (!SD.begin(CS_PIN))
    {
        Serial.println ("SD initialisation failed!");
        led_blink_signal (ERRORS_SD_CARD_FAILED_INIT);
    }

    datafile = SD.open("ezmometr.bin", O_READ);

    datafile.read(&memoryHeader, sizeof (memoryHeader));
    datafile.close();

    latest_data.set       = memoryHeader.last_set;
    latest_data.abs_index = memoryHeader.total_measures;

#if DEBUG_INIT
    Serial.println ("-- INIT: Memory");
#endif
}

void memory_prepare_sampling ()
{
    datafile = SD.open("ezmometr.bin", O_READ | O_WRITE);
    datafile.seek (sizeof (memory_header) + memoryHeader.total_measures * sizeof (filtered_data));
}

void memory_sample ()
{
    datafile.write ((const uint8_t *) &latest_data, sizeof (filtered_data));
    latest_data.set_index++;
    latest_data.abs_index++;
}

void memory_fetch (filtered_data *buf, uint32_t abs_pos)
{
    Serial.print ("Fetching at logical pos ");
    Serial.println (abs_pos);

    datafile = SD.open ("ezmometr.bin", O_READ);
    datafile.seek (sizeof (memory_header) + abs_pos * sizeof (filtered_data));
    datafile.read (buf, sizeof (filtered_data));
    datafile.close();

    Serial.print ("Fetched logical pos:");
    Serial.print (buf->abs_index);
}

void memory_stop_sampling ()
{
    datafile.seek(0);

    memoryHeader.total_measures = latest_data.abs_index;
    memoryHeader.last_set       = latest_data.set;
    datafile.write((const uint8_t *) &memoryHeader, sizeof(memoryHeader));
    datafile.close();
}