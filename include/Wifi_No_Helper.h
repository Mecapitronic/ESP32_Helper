/**
 * @file Wifi_No_Helper.h
 * @author Mecapitronic (mecapitronic@gmail.com)
 * @brief Used to connect to replace wifi if not compiled with
 * @date 2023-07-25
 */

#ifndef WIFI_NO_HELPER_H
#define WIFI_NO_HELPER_H
#ifndef WITH_WIFI
#include "ESP32_Helper.h"

namespace Wifi_Helper
{
    bool IsOTARunning();
    void EnableWifi(Enable enable);
    bool IsEnable();

    void Initialisation(void);
    void Update(void *pvParameters);
    void HandleCommand(Command cmdTmp);
    void PrintCommandHelp(void);
    
    void SetLocalIP(String ip);
    void SetServerIP(String ip);
}
#endif
#endif
