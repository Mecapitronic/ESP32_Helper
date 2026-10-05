#include "ESP32_Helper.h"
//#include <limits>

using namespace Printer;

String jsonFileName;
void setup(void)
{
    ESP32_Helper::Initialisation();
    jsonFileName = "simple.json";
}

void loop(void)
{
    println("Reading initial JSON");
    JsonDocument document;
    if (JSON_Helper::LoadJsonFile(document, jsonFileName))
    {
        String nom = document["nom"].as<String>();
        int32_t compteur = document["compteur"].as<int32_t>();
        bool actif = document["actif"].as<bool>();
        println("nom = %s, compteur = %ld, actif = %s",
                nom.c_str(), static_cast<long>(compteur), actif ? "true" : "false");

        if (compteur == std::numeric_limits<int32_t>::max())
        {
            println("Cannot increment compteur: integer limit reached");
            return;
        }
        document["compteur"] = ++compteur;
        if (document.overflowed())
        {
            println("JSON update failed: insufficient memory");
            return;
        }

        String serialized;
        size_t bytesWritten = serializeJsonPretty(document, serialized);
        if (bytesWritten == 0 || bytesWritten != measureJsonPretty(document) ||
            serialized.length() != bytesWritten)
        {
            println("JSON serialization failed or incomplete");
            return;
        }
        FileSystem_Helper::WriteFile(jsonFileName, serialized);

        println("Reading saved JSON");
        JsonDocument savedDocument;
        if (!JSON_Helper::LoadJsonFile(savedDocument, jsonFileName))
        {
            return;
        }
        if (savedDocument["nom"].as<String>() != nom ||
            savedDocument["compteur"].as<int32_t>() != compteur ||
            savedDocument["actif"].as<bool>() != actif)
        {
            println("JSON verification failed: saved values differ");
            return;
        }

        println("JSON round-trip verified: compteur = %ld", static_cast<long>(compteur));
    }
    else
        println("Failed to load JSON file: %s", jsonFileName.c_str());
    
    delay(3000);
}