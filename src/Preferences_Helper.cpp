#include "Preferences_Helper.h"

using namespace Printer;
using namespace std;

namespace Preferences_Helper
{
    Preferences preferences;
    vector<String> preferencesList;
    const char * const PREF_NAMESPACE = "Mecapi";

    int32_t LoadFromPreference(const String &pref, int32_t defValue)
    {
        preferences.begin(PREF_NAMESPACE, true);
        int32_t value = preferences.getInt(pref.c_str(), defValue);
        preferencesList.push_back(pref);
        println("Preferences " + pref + " = " + String(value));
        preferences.end();
        return value;
    }

    String LoadFromPreference(const String &pref, const String &defValue)
    {
        preferences.begin(PREF_NAMESPACE, true);
        String value = preferences.getString(pref.c_str(), defValue.c_str());
        preferencesList.push_back(pref);
        println("Preferences " + pref + " = " + value);
        preferences.end();
        return value;
    }

    void SaveToPreference(const String &pref, int32_t value)
    {
        preferences.begin(PREF_NAMESPACE, false);
        preferences.putInt(pref.c_str(), value);
        preferencesList.push_back(pref);
        preferences.end();
    }

    void SaveToPreference(const String &pref, const String &value)
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
                println("Clear all pref under: " + String(PREF_NAMESPACE));
                Preferences_Helper::ClearPreferences();
            }
        }
        if(cmdTmp.cmd == "PreferencesList")
        {
            println("Preferences List : ");
            for (size_t i = 0; i < preferencesList.size(); i++)
            {
                if(preferences.isKey(preferencesList[i].c_str()))
                {
                    PreferenceType type = preferences.getType(preferencesList[i].c_str());
                    println("  " + preferencesList[i] + " - Type: " + String((int)type));
                }
                else
                {
                    println("  " + preferencesList[i] + " - Key not found.");
                }
            }
        }
        else
        {
            println("Not a Preferences command !");            
            PrintCommandHelp();
        }
    }
    
    void PrintCommandHelp()
    {
        println("Preferences Command Help :");
        println(" > PreferencesClear:[int]");
        println("      1 Clear all preferences under the namespace");
        println(" > PreferencesList");
        println("      List all key preferences");
    }
}
