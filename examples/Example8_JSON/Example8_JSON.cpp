#include "ESP32_Helper.h"
#include "SimpleData.h"

using namespace Printer;

const String jsonFileName = "simple.json";
SimpleData simpleData;

// ============== Custom Commands ==============

void RegisterMyCustomCommands()
{
    ESP32_Helper::RegisterCommand("SimpleShow", "SimpleShow - Show current values.", FunctionCommand( simpleData.Print(); ));
    ESP32_Helper::RegisterCommand("SimpleNom", "SimpleNom:<name:string> - Set nom in memory; use SimpleSave to persist.", FunctionCommand(
        if (!cmd.hasDataStr1())
            return;
        simpleData.nom = String(cmd.dataStr1);
    ));
    ESP32_Helper::RegisterCommand("SimpleCompteur", "SimpleCompteur:<value:int> - Set compteur in memory; value >= 0. Use SimpleSave to persist.", FunctionCommand(
        if (!cmd.hasData() || cmd.data[0] < 0)
            return;
        simpleData.compteur = cmd.data[0];
    ));
    ESP32_Helper::RegisterCommand("SimpleActif", "SimpleActif:<enabled:bool> - Set actif in memory; use SimpleSave to persist.", FunctionCommand(
        if (!cmd.hasData())
            return;
        simpleData.actif = cmd.data[0] != 0;
    ));
    ESP32_Helper::RegisterCommand("SimpleLoad", "SimpleLoad - Reload values from the JSON file.", FunctionCommand(
        if (JSON_Helper::LoadObject(simpleData, jsonFileName))
            println("Loaded from %s", jsonFileName.c_str());
    ));
    ESP32_Helper::RegisterCommand("SimpleSave", "SimpleSave - Save current values to the JSON file.", FunctionCommand(
        if (JSON_Helper::SaveObject(simpleData, jsonFileName))
           println("Saved to %s", jsonFileName.c_str());
    ));
}

// ============== Setup ==============

void setup(void)
{
    ESP32_Helper::Initialisation();

    RegisterMyCustomCommands();

    if (!JSON_Helper::LoadObject(simpleData, jsonFileName))
    {
        println("Creating %s with default values", jsonFileName.c_str());
        JSON_Helper::SaveObject(simpleData, jsonFileName);
    }
    simpleData.Print();
}

// ============== Loop ==============

void loop(void)
{
    if (simpleData.actif)
    {
        simpleData.compteur++;
        println("Compteur = %ld", static_cast<long>(simpleData.compteur));
    }

    delay(3000);
}