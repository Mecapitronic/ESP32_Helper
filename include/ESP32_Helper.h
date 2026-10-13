/**
 * @file ESP32_Helper.h
 * @author Mecapitronic (mecapitronic@gmail.com)
 * @brief
 * @date 2023-07-25
 */
#ifndef ESP32_HELPER_H
#define ESP32_HELPER_H

#include "Structure_Helper.h"

#include "Wifi_Helper.h"
#ifdef WITH_WIFI
#define WIFI_DEBUG Wifi_Helper::wifiClient
#else
#define WIFI_DEBUG Serial
#endif

#ifdef SERIAL_DEBUG
#undef SERIAL_DEBUG
#endif
#define SERIAL_DEBUG Serial

#include "Preferences_Helper.h"
#include "QueueThread.h"
#include "TaskThread.h"
#include "TimerThread.h"
#include "Logger.h"
#include "Printer.h"
#include "Debugger.h"
#include "FileSystem_Helper.h"
#include "JSON_Helper.h"

namespace ESP32_Helper
{
    /**
     * Initialize serial/wifi for PC communication
     */
    void Initialisation(BaudRate baud_speed = BaudRate::BAUD_RATE_921600, Enable printEnable = Enable::ENABLE_TRUE,
                        Level printLvl = Level::LEVEL_VERBOSE, Enable debugEnable = Enable::ENABLE_FALSE);
    void Update(void *pvParameters);
    void ProcessIncomingChar(char c);
    bool HandleCommand(Command cmdTmp);
    Command ParseCommandBuffer(const std::vector<char> &read);

    /**
     * @brief Register a custom command handler
     * @param command   Command to match (e.g., "MyCmd")
     * @param help      Help text describing the command
     * @param func      Function pointer to handle the command
     */
    void RegisterCommand(const String &command, const String &help, CommandHandlerFunc func);
    
    bool HasWaitingCommand();
    Command GetCommand();
    String convertToString(char *a, int32_t size);

}
#endif
