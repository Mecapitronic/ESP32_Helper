/**
 * @file Wifi_Helper.h
 * @author Mecapitronic (mecapitronic@gmail.com)
 * @brief Used to connect to wifi access point, connect to server and override Serial print
 * @date 2023-07-25
 */

#ifndef WIFI_HELPER_H
#define WIFI_HELPER_H
#include "ESP32_Helper.h"

#ifdef WITH_WIFI
#include <WiFi.h>

#ifdef WITH_OTA
#include <ArduinoOTA.h>
#endif
#endif

namespace Wifi_Helper
{
    
#ifdef WITH_WIFI
    extern WiFiClient wifiClient;
    void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
    void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
#endif

    bool IsOTARunning();
    void EnableWifi(Enable enable);
    bool IsEnable();
    bool IsWifiConnected();
    bool IsClientConnected();

    void Initialisation(void);
    void Update(void *pvParameters);
    void ProcessIncomingChar(char c);
    void HandleWifiConnection();
    void HandleClientConnection();
    void HandleTeleplotConnection();

    bool HandleCommand(Command cmdTmp);
    void PrintCommandHelp(void);

    void SetLocalIP(const String &ip);
    void SetLocalIP(int lastDigit);
    void SetServerIP(const String &ip);
    void SetServerIP(int lastDigit);
    void SetTeleplotIP(const String &ip);
    void SetTeleplotIP(int lastDigit);
}
#endif
