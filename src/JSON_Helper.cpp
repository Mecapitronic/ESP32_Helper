#include "JSON_Helper.h"

using namespace Printer;
using namespace std;

namespace JSON_Helper
{
    bool LoadJsonFile(JsonDocument &document, const String &fileName)
    {
        String content = FileSystem_Helper::ReadFile(fileName);
        if (content.isEmpty())
        {
            println("JSON file missing, empty or unreadable");
            return false;
        }

        DeserializationError error = deserializeJson(document, content);
        if (error)
        {
            println("JSON deserialization failed: %s", error.c_str());
            return false;
        }
        return true;
    }

    bool SaveJsonFile(const JsonDocument &document, const String &fileName)
    {
        if (document.overflowed())
        {
            println("JSON save failed: insufficient memory");
            return false;
        }

        String serialized;
        size_t bytesWritten = serializeJsonPretty(document, serialized);
        if (bytesWritten == 0 || serialized.length() != bytesWritten)
        {
            println("JSON serialization failed or incomplete");
            return false;
        }
        FileSystem_Helper::WriteFile(fileName, serialized, true);
        return true;
    }

    void PrintVerificationError(const String &fileName)
    {
        println("JSON verification failed: saved values differ in %s", fileName.c_str());
    }
}