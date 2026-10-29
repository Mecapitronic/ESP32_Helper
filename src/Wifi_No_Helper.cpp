#ifndef WITH_WIFI
#include "Wifi_Helper.h"

namespace Wifi_Helper
{
    namespace
    {
        Enable wifiEnable = Enable::ENABLE_NONE;
    }

    bool IsOTARunning() { return false; }

    void EnableWifi(Enable enable)
    {
        wifiEnable = Enable::ENABLE_FALSE;
        Printer::println("WIFI is Disable");
    }

    bool IsEnable() { return false; }

    bool IsWifiConnected() { return false; }

    bool IsClientConnected() { return false; }

    void Initialisation()
    {
        Printer::println("-- NO Wifi initialisation --");
        RegisterCommands();
    }
    
    void RegisterCommands()
    {        
        ESP32_Helper::RegisterCommand("Wifi", "Wifi - Report that Wi-Fi is not available in this build.", FunctionCommand( PrintWifiStatus(); ));
    }

    void Update(void *pvParameters)
    {
        PrintWifiStatus();
    }
    
    void SetSsid(const String &ssid)
    {
        PrintWifiStatus();
    }

    void SetPassword(const String &password)
    {
        PrintWifiStatus();
    }

    void SetBaseIP(int first, int second, int third)
    {
        PrintWifiStatus();
    }

    void SetLocalIP(const String &ip)
    {
        PrintWifiStatus();
    }

    void SetLocalIP(int lastDigit)
    {
        PrintWifiStatus();
    }
    
    void SetServerIP(const String &ip)
    {
        PrintWifiStatus();
    }

    void SetServerIP(int lastDigit)
    {
        PrintWifiStatus();
    }
    
    void SetTeleplotIP(const String &ip)
    {
        PrintWifiStatus();
    }

    void SetTeleplotIP(int lastDigit)
    {
        PrintWifiStatus();
    }
    
    void SetServerPort(int port)
    {
        PrintWifiStatus();
    }

    void PrintWifiStatus()
    {
        Printer::println("NO WIFI !");
    }
}
#endif
