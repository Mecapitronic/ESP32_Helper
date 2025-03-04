#include "Preferences_Helper.h"

namespace Preferences_Helper
{
    Preferences preferences;
    vector<String> preferencesList;
    const char * PREF_NAMESPACE = "Mecapi";

    int LoadFromPreference(String pref, int defValue)
    {
        preferences.begin(PREF_NAMESPACE, true);
        int value = preferences.getInt(pref.c_str(), defValue);
        preferencesList.push_back(pref);
        Printer::print("Preferences ");
        Printer::println(pref, value);
        preferences.end();
        return value;
    }

    String LoadFromPreference(String pref, String defValue)
    {
        preferences.begin(PREF_NAMESPACE, true);
        String value = preferences.getString(pref.c_str(), defValue.c_str());
        preferencesList.push_back(pref);
        Printer::print("Preferences ");
        Printer::println(pref, "=", value);
        preferences.end();
        return value;
    }

    void SaveToPreference(String pref, int value)
    {
        preferences.begin(PREF_NAMESPACE, false);
        preferences.putInt(pref.c_str(), value);
        preferencesList.push_back(pref);
        preferences.end();
    }

    void SaveToPreference(String pref, String value)
    {
        preferences.begin(PREF_NAMESPACE, false);
        preferences.putString(pref.c_str(), value.c_str());
        preferencesList.push_back(pref);
        preferences.end();
    }

    void ClearPreferences()
    {
        preferences.begin(PREF_NAMESPACE, false);
        preferences.clear();
        preferencesList.clear();
        preferences.end();
    }
    
    void HandleCommand(Command cmdTmp)
    {
        if (cmdTmp.cmd == "PreferencesClear")
        {
            // PreferencesClear:1
            if (cmdTmp.size == 1 && cmdTmp.data[0] == 1)
            {
                Printer::print("Clear all pref under : ");
                Printer::println(PREF_NAMESPACE);
                Preferences_Helper::ClearPreferences();
            }
        }
        if(cmdTmp.cmd == "PreferencesList")
        {
            Printer::println("Preferences List : ");
            for (size_t i = 0; i < preferencesList.size(); i++)
            {
                if(preferences.isKey(preferencesList[i].c_str()))
                {
                    PreferenceType type = preferences.getType(preferencesList[i].c_str());
                    Printer::println(preferencesList[i]);
                    Printer::print(" - Type of ");
                    Printer::print(type);
                }
                else
                {
                    Printer::println(preferencesList[i] + " - Key not found.");
                }
            }
        }
    }
    
    void PrintCommandHelp()
    {
        Printer::println("Preferences Command Help :");
        Printer::println(" > PreferencesClear:[int]");
        Printer::println("      1 Clear all preferences under the namespace");
        Printer::println(" > PreferencesList");
        Printer::println("      List all key preferences");
    }
}
