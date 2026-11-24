#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#include "ESP32_Helper.h"
#include <ArduinoJson.h>

class IJsonSerializable
{
public:
    virtual ~IJsonSerializable() = default;

    virtual bool FromJson(const JsonDocument &document) = 0;
    virtual void ToJson(JsonDocument &document) const = 0;
};

namespace JSON_Helper
{
    bool LoadJsonFile(JsonDocument &document, const String &fileName);
    bool SaveJsonFile(const JsonDocument &document, const String &fileName);

    bool LoadObject(IJsonSerializable &object, const String &fileName);
    bool SaveObject(const IJsonSerializable &object, const String &fileName);
}
#endif