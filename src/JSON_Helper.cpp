#include "JSON_Helper.h"

using namespace Printer;
using namespace std;

namespace JSON_Helper
{
    namespace
    {
        const char *VERSION_KEY = "version";

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
        if (!LoadJsonFile(document, fileName))
            return false;

        uint16_t fileVersion = document[VERSION_KEY] | 0;
        if (fileVersion != object.Version())
        {
            if (!object.Migrate(document, fileVersion))
            {
                println("JSON unsupported version %u in %s, expected %u",
                        fileVersion, fileName.c_str(), object.Version());
                return false;
            }
            println("JSON %s migrated from version %u to %u", fileName.c_str(), fileVersion, object.Version());
        }
        return object.FromJson(document);
    }

    bool SaveObject(const IJsonSerializable &object, const String &fileName)
    {
        JsonDocument document;
        object.ToJson(document);
        document[VERSION_KEY] = object.Version();

        String serialized;
        if (!Serialize(document, serialized) || !FileSystem_Helper::WriteFile(fileName, serialized, true))
            return false;

        if (FileSystem_Helper::ReadFile(fileName) != serialized)
        {
            println("JSON verification failed: saved content differs in %s", fileName.c_str());
            return false;
        }
        return true;
    }
}