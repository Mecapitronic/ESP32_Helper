/**
 * @file ESP32_Helper.h
 * @author Mecapitronic (mecapitronic@gmail.com)
 * @brief
 * @date 2023-07-25
 */
#ifndef ESP32_HELPER_H
#define ESP32_HELPER_H

#include "Structure.h"
#include "Preferences_Helper.h"

#ifdef WITH_WIFI
    #include "Wifi_Helper.h"
    //extern WiFiClient wifiClient;
    #define WIFI_DEBUG Wifi_Helper::wifiClient
#else
    #include "Wifi_No_Helper.h"
    #define WIFI_DEBUG Serial
#endif

#ifndef SERIAL_DEBUG
    #define SERIAL_DEBUG Serial
#endif

#include "Printer.h"
#include "Debugger.h"

namespace ESP32_Helper
{

// ESP32 Serial Bauds rates
// static const unsigned long default_rates[] = {300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 74880,
// 115200, 230400, 256000, 460800, 921600, 1843200, 3686400};

/**
 * Initialize serial/wifi for PC communication
 */
void Initialisation(int baud_speed = 921600, Enable printEnable = ENABLE_TRUE,
                  Level printLvl = LEVEL_VERBOSE, Enable debugEnable = ENABLE_FALSE);
void Update(void *pvParameters);
void HandleCommand(Command cmdTmp);
void BufferReadCommand(char * read, int size);

bool HasWaitingCommand();
Command GetCommand();

/**
 * @brief Get the current time with miliseconds precision
 *
 * @return int64_t current time in miliseconds
 */
int64_t GetTimeNowMs();

/**
 * @brief Get the current time with microsecond precision
 *
 * @return int64_t current time in microseconds
 */
int64_t GetTimeNowUs();
}
#endif
