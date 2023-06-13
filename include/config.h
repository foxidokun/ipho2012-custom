#ifndef IPHO2012_REWORKED_CONFIG_H
#define IPHO2012_REWORKED_CONFIG_H

/// Пины кнопок. Все кнопки должны быть подключены к одному и тому же регистру
#define BUTTON_START_BUTTON_PIN  2
#define BUTTON_STOP_BUTTON_PIN   3
#define BUTTON_PREV_BUTTON_PIN   4
#define BUTTON_NEXT_BUTTON_PIN   5
#define BUTTON_SAMPLE_BUTTON_PIN 6

/// Сколько измерений использовать при расчете производной
#define FILTER_MAX_HISTORY 3
/// Кол-во повторных измерений на точку (больше -- дольше)
#define NUM_SAMPLES_PER_MEASUREMENT 3
/// Сопротивление резистора (Ом)
#define RESISTOR 1

/// Пауза между битами в сигнале ошибки
#define LED_INTERNAL_PAUSE_MS 300
/// Пауза между повторами сигнала
#define LED_WORD_PAUSE_MS 700
/// Пин, на котором находится светодиод
#define LED_PIN 8

#define FLASH_HOLD_PIN 15 // A1
#define FLASH_WP_PIN   14 // A0
#define FLASH_CE_PIN   10

/// Как часто снимать точки
/// !! Важно: обработка производных завязана на то, что точки снимают каждые N миллисекунд, так что
/// такт цикла должен укладываться в это время.
/// Чтобы проверить совместимость параметров MEASURE_MILLIS и NUM_SAMPLES_PER_MEASUREMENT помогает опция
/// DEBUG_RUNAWAY_DETECT, которая выводит сообщение в консоль если был пропущен момент измерения
#define MEASURE_MILLIS 100

#endif //IPHO2012_REWORKED_CONFIG_H
