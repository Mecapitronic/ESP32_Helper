#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#include "ESP32_Helper.h"
#include <ArduinoJson.h>

namespace JSON_Helper
{
    bool LoadJsonFile(JsonDocument &document, const String &fileName);
    bool SaveJsonFile(const JsonDocument &document, const String &fileName);
    void PrintVerificationError(const String &fileName);

    // T must provide: bool FromJson(const JsonDocument&), void ToJson(JsonDocument&) const, operator==
    template <typename T>
    bool LoadObject(T &object, const String &fileName)
    {
        JsonDocument document;
        return LoadJsonFile(document, fileName) && object.FromJson(document);
    }

    template <typename T>
    bool SaveObject(const T &object, const String &fileName)
    {
        JsonDocument document;
        object.ToJson(document);
        if (!SaveJsonFile(document, fileName))
            return false;

        T saved;
        if (!LoadObject(saved, fileName) || !(saved == object))
        {
            PrintVerificationError(fileName);
            return false;
        }
        return true;
    }
}
#endif