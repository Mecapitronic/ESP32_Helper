#ifndef SIMPLE_DATA_H
#define SIMPLE_DATA_H

#include "ESP32_Helper.h"

// Mirror of data/simple.json
struct SimpleData
{
    String nom = "";
    int32_t compteur = 0;
    bool actif = false;

    bool FromJson(const JsonDocument &document)
    {
        if (!document["nom"].is<String>() ||
            !document["compteur"].is<int32_t>() ||
            !document["actif"].is<bool>())
        {
            Printer::println("Invalid JSON: expected nom (string), compteur (integer), actif (boolean)");
            return false;
        }
        if (document["compteur"].as<int32_t>() < 0)
        {
            Printer::println("Invalid JSON: compteur must be non-negative");
            return false;
        }

        nom = document["nom"].as<String>();
        compteur = document["compteur"].as<int32_t>();
        actif = document["actif"].as<bool>();
        return true;
    }

    void ToJson(JsonDocument &document) const
    {
        document["nom"] = nom;
        document["compteur"] = compteur;
        document["actif"] = actif;
    }

    bool operator==(const SimpleData &other) const
    {
        return nom == other.nom && compteur == other.compteur && actif == other.actif;
    }

    void Print() const
    {
        Printer::println("nom = %s, compteur = %ld, actif = %s",
                         nom.c_str(), static_cast<long>(compteur), actif ? "true" : "false");
    }
};

#endif
