#include "ESP32_Helper.h"
#include "SimpleData.h"

using namespace Printer;

const String jsonFileName = "simple.json";
SimpleData simpleData;

// ============== Custom Commands ==============

void RegisterMyCustomCommands()
{
    ESP32_Helper::RegisterCommand("SimpleShow", "Print current values", FunctionCommand( simpleData.Print(); ));
    ESP32_Helper::RegisterCommand("SimpleNom", "SimpleNom:text - Set nom and save", FunctionCommand(
        if (!cmd.hasDataStr1())
            return;
        simpleData.nom = String(cmd.dataStr1);
    ));
    ESP32_Helper::RegisterCommand("SimpleCompteur", "SimpleCompteur:value - Set compteur and save", FunctionCommand(
        if (!cmd.hasData() || cmd.data[0] < 0)
            return;
        simpleData.compteur = cmd.data[0];
    ));
    ESP32_Helper::RegisterCommand("SimpleActif", "SimpleActif:0|1 - Set actif and save", FunctionCommand(
        if (!cmd.hasData())
            return;
        simpleData.actif = cmd.data[0] != 0;
    ));
    ESP32_Helper::RegisterCommand("SimpleLoad", "Reload values from file", FunctionCommand(
        if (JSON_Helper::LoadObject(simpleData, jsonFileName))
            println("Loaded from %s", jsonFileName.c_str());
    ));
    ESP32_Helper::RegisterCommand("SimpleSave", "Save current values to file", FunctionCommand(
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