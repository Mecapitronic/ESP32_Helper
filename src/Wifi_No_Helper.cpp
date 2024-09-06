#ifndef WITH_WIFI
#include "Wifi_No_Helper.h"

namespace Wifi_Helper
{
    namespace
    {
        Enable wifiEnable = ENABLE_NONE;
    }

    void EnableWifi(Enable enable)
    {
        wifiEnable = ENABLE_FALSE;
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
}
#endif
