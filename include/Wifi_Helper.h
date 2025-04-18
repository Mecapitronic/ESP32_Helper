/**
 * @file Wifi_Helper.h
 * @author Mecapitronic (mecapitronic@gmail.com)
 * @brief Used to connect to wifi access point, connect to server and override Serial print
 * @date 2023-07-25
 */

#ifndef WIFI_HELPER_H
#define WIFI_HELPER_H
#ifdef WITH_WIFI
#include <WiFi.h>
#include "ESP32_Helper.h"

#ifdef WITH_OTA
#include <ArduinoOTA.h>
#endif

namespace Wifi_Helper
{
    extern WiFiClient wifiClient;

    void EnableWifi(Enable enable);
    bool IsEnable();

    void Initialisation(void);
    void Update(void *pvParameters);
    void ProcessIncomingChar(char c);
    void HandleWifiConnection();
    void HandleClientConnection();
    void HandleTeleplotConnection();

    void HandleCommand(Command cmdTmp);
    void PrintCommandHelp(void);

    void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
    void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
}
#endif
#endif
