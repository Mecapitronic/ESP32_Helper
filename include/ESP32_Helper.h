/**
 * @file ESP32_Helper.h
 * @author Mecapitronic (mecapitronic@gmail.com)
 * @brief
 * @date 2023-07-25
 */
#ifndef ESP32_HELPER_H
#define ESP32_HELPER_H

#include "Structure_Helper.h"
#include "Preferences_Helper.h"
#include "QueueThread.h"
#include "TaskThread.h"

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
/**
 * Initialize serial/wifi for PC communication
 */
void Initialisation(BaudRate baud_speed = BaudRate::BAUD_RATE_921600, Enable printEnable = Enable::ENABLE_TRUE,
                  Level printLvl = Level::LEVEL_VERBOSE, Enable debugEnable = Enable::ENABLE_FALSE);
void Update(void *pvParameters);
void HandleCommand(Command cmdTmp);
void BufferReadCommand(char *read, int32_t size);

bool HasWaitingCommand();
Command GetCommand();
}
#endif
