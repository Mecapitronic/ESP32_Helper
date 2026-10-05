#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#include "ESP32_Helper.h"
#include <ArduinoJson.h>

namespace JSON_Helper
{    
    bool Initialisation();

    bool LoadJsonFile(JsonDocument &document, const String &fileName);
    bool HandleCommand(Command cmdTmp);

    void PrintCommandHelp();
}
#endif