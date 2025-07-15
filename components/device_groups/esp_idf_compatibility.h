#ifndef ESP_IDF_COMPATIBILITY_H
#define ESP_IDF_COMPATIBILITY_H

#if defined(USE_ESP_IDF)

// ESP-IDF compatibility for Arduino PROGMEM functions
#define PSTR(str) (str)

// ESP-IDF compatibility for sprintf_P (PROGMEM version of sprintf)
#define sprintf_P sprintf

// ESP-IDF compatibility for snprintf_P (PROGMEM version of snprintf)
#define snprintf_P snprintf

// ESP-IDF compatibility for strncmp_P (PROGMEM version of strncmp)
#define strncmp_P strncmp

#endif // USE_ESP_IDF

#endif // ESP_IDF_COMPATIBILITY_H
