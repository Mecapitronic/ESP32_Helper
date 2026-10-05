#include "JSON_Helper.h"

using namespace Printer;
using namespace std;

namespace JSON_Helper
{
    bool Initialisation()
    {
        return true;
    }

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

        if (!document.is<JsonObject>() ||
            !document["nom"].is<String>() ||
            !document["compteur"].is<int32_t>() ||
            !document["actif"].is<bool>())
        {
            println("Invalid JSON: expected nom (string), compteur (integer), actif (boolean)");
            return false;
        }
        if (document["compteur"].as<int32_t>() < 0)
        {
            println("Invalid JSON: compteur must be non-negative");
            return false;
        }

        println(content);
        return true;
    }

    bool HandleCommand(Command cmdTmp)
    {
        if (cmdTmp.cmdEquals("JSON"))
        {
            // JSON
            //JSON();
        }
        else
        {
            println("Not a JSON command !");            
            return false;
        }
        return true;
    }
    
    void PrintCommandHelp()
    {
        println("JSON Command Help");
        println(" > JSON");
        println("      JSON");
        println();
    }
}