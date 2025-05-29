#ifndef WITH_WIFI
#include "Wifi_No_Helper.h"

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

    void Initialisation()
    {
        Printer::println("-- NO Wifi initialisation --");
    }

    void Update(void *pvParameters)
    {
        Printer::println("Wifi Update Task STOPPED !");
    }

    void HandleCommand(Command cmdTmp)
    {
        Printer::println("NO WIFI !");
    }
    
    void PrintCommandHelp()
    {
        Printer::println("NO WIFI !");
    }
    
    void SetLocalIP(String ip)
    {
        Printer::println("NO WIFI !");
    }
    
    void SetServerIP(String ip)
    {
        Printer::println("NO WIFI !");
    }
}
#endif
