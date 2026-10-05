#ifndef SIMPLE_DATA_H
#define SIMPLE_DATA_H

#include "ESP32_Helper.h"

// Mirror of data/simple.json
struct SimpleData : public IJsonSerializable
{
    String nom = "";
    int32_t compteur = 0;
    bool actif = false;

    uint16_t Version() const override { return 1; }

    // Version 0 = file written before the "version" field existed, same content
    bool Migrate(JsonDocument & /*document*/, uint16_t fromVersion) override
    {
        return fromVersion == 0;
    }

    bool FromJson(const JsonDocument &document) override
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

    void ToJson(JsonDocument &document) const override
    {
        document["nom"] = nom;
        document["compteur"] = compteur;
        document["actif"] = actif;
    }

    void Print() const
    {
        Printer::println("nom = %s, compteur = %ld, actif = %s",
                         nom.c_str(), static_cast<long>(compteur), actif ? "true" : "false");
    }
};

#endif
