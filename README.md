### Файлы, достойные внимания
* `include/debug.h`  -- Переключатели дебажных настроек. Стоит выставить все в 0
* `include/error.h`  -- Соответствие паттернов внутреннего различным светодиода ошибкам (в текущий момент не работает из-за конфликта пинов)
* `include/config.h` -- Все настраиваемые define'ы

### Схема подключения Arduino UNO
* A4 -- ADS1115 SDA
* A5 -- ADS1115 SCL
* 2  -- Start button
* 3  -- Stop button
* 4  -- Prev button
* 5  -- Next button
* 6  -- Sample button
* 10 -- SD Card CS
* 11 -- SD Card D1
* 12 -- SD Card D0
* 13 -- SD Card SCL

### Требования к SD карте для первого запуска
* FAT16/32 раздел с файлом "ezmometr.bin" (размер > 10 байт, заполнен нулями)