#include "JSON_Helper.h"

using namespace Printer;
using namespace std;

namespace JSON_Helper
{
    namespace
    {
        bool Serialize(const JsonDocument &document, String &serialized)
        {
            if (document.overflowed())
            {
                println("JSON save failed: insufficient memory");
                return false;
            }

            size_t bytesWritten = serializeJsonPretty(document, serialized);
            if (bytesWritten == 0 || serialized.length() != bytesWritten)
            {
                println("JSON serialization failed or incomplete");
                return false;
            }
            return true;
        }
    }

    bool LoadJsonFile(JsonDocument &document, const String &fileName)
    {
        if (!SPIFFS.exists("/" + fileName))
        {
            println("JSON file missing: %s", fileName.c_str());
            return false;
        }
        File file = SPIFFS.open("/" + fileName);
        if (!file)
        {
            println("JSON file unreadable: %s", fileName.c_str());
            return false;
        }

        DeserializationError error = deserializeJson(document, file);
        file.close();
        if (error)
        {
            println("JSON deserialization failed: %s", error.c_str());
            return false;
        }
        return true;
    }

    bool SaveJsonFile(const JsonDocument &document, const String &fileName)
    {
        String serialized;
        return Serialize(document, serialized) && FileSystem_Helper::WriteFile(fileName, serialized, true);
    }

    bool LoadObject(IJsonSerializable &object, const String &fileName)
    {
        JsonDocument document;
        return LoadJsonFile(document, fileName) && object.FromJson(document);
    }

    bool SaveObject(const IJsonSerializable &object, const String &fileName)
    {
        JsonDocument document;
        object.ToJson(document);
        return SaveJsonFile(document, fileName);
    }
}