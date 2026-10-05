#include "ESP32_Helper.h"
#include "SimpleData.h"

using namespace Printer;

const String jsonFileName = "simple.json";
SimpleData simpleData;

// ============== Custom Command Handler ==============

void PrintSimpleHelp()
{
    println("Simple JSON Commands");
    println(" > SimpleShow");
    println("      Print current values");
    println(" > SimpleNom:text");
    println("      Set nom and save");
    println(" > SimpleCompteur:value");
    println("      Set compteur and save");
    println(" > SimpleActif:0|1");
    println("      Set actif and save");
    println(" > SimpleLoad");
    println("      Reload values from file");
    println();
}

bool HandleSimpleCommand(Command cmd)
{
    if (cmd.cmdEquals("SimpleShow"))
    {
        simpleData.Print();
        return true;
    }
    else if (cmd.cmdEquals("SimpleLoad"))
    {
        if (JSON_Helper::LoadObject(simpleData, jsonFileName))
            simpleData.Print();
        return true;
    }
    else if (cmd.cmdEquals("SimpleNom") && cmd.dataStr1[0] != '\0')
    {
        simpleData.nom = String(cmd.dataStr1);
    }
    else if (cmd.cmdEquals("SimpleCompteur") && cmd.size == 1 && cmd.data[0] >= 0)
    {
        simpleData.compteur = cmd.data[0];
    }
    else if (cmd.cmdEquals("SimpleActif") && cmd.size == 1)
    {
        simpleData.actif = cmd.data[0] != 0;
    }
    else
    {
        return false;
    }

    if (JSON_Helper::SaveObject(simpleData, jsonFileName))
        simpleData.Print();
    return true;
}

// ============== Setup ==============

void setup(void)
{
    ESP32_Helper::Initialisation();

    ESP32_Helper::RegisterCommandHandler("Simple", HandleSimpleCommand, PrintSimpleHelp);

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
        if (JSON_Helper::SaveObject(simpleData, jsonFileName))
            println("JSON round-trip verified: compteur = %ld", static_cast<long>(simpleData.compteur));
    }

    delay(3000);
}
