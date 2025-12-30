#include "Preferences_Helper.h"

using namespace Printer;
using namespace std;

namespace Preferences_Helper
{
    Preferences preferences;
    // This list is not perfect because if you save a key manually by command and reboot, it won't be in the list
    vector<String> preferencesList;
    const char * const PREF_NAMESPACE = "Mecapi";

    int32_t LoadFromPreference(const String &pref, int32_t defValue)
    {
        preferences.begin(PREF_NAMESPACE, true);
        int32_t value = preferences.getInt(pref.c_str(), defValue);
        if(preferencesList.size() == 0 || std::find(preferencesList.begin(), preferencesList.end(), pref) == preferencesList.end())
            preferencesList.push_back(pref);
        println("Load Preferences : " + pref + " = " + String(value));
        preferences.end();
        return value;
    }

    String LoadFromPreference(const String &pref, const String &defValue)
    {
        preferences.begin(PREF_NAMESPACE, true);
        String value = preferences.getString(pref.c_str(), defValue.c_str());
        if(preferencesList.size() == 0 || std::find(preferencesList.begin(), preferencesList.end(), pref) == preferencesList.end())
            preferencesList.push_back(pref);
        println("Load Preferences : " + pref + " = " + value);
        preferences.end();
        return value;
    }

    void SaveToPreference(const String &pref, int32_t value)
    {
        preferences.begin(PREF_NAMESPACE, false);
        preferences.putInt(pref.c_str(), value);
        if(preferencesList.size() == 0 || std::find(preferencesList.begin(), preferencesList.end(), pref) == preferencesList.end())
            preferencesList.push_back(pref);
        println("Save Preferences : " + pref + " = " + value);
        preferences.end();
    }

    void SaveToPreference(const String &pref, const String &value)
    {
        preferences.begin(PREF_NAMESPACE, false);
        preferences.putString(pref.c_str(), value.c_str());
        if(preferencesList.size() == 0 || std::find(preferencesList.begin(), preferencesList.end(), pref) == preferencesList.end())
            preferencesList.push_back(pref);
        println("Save Preferences : " + pref + " = " + value);
        preferences.end();
    }

    void RemoveFromPreference(const String &pref)
    {
        preferences.begin(PREF_NAMESPACE, false);
        bool deleted = preferences.remove(pref.c_str());
        auto it = std::find(preferencesList.begin(), preferencesList.end(), pref);
        if(it != preferencesList.end())
            preferencesList.erase(it);
        println("Remove Preferences : %s %s", pref.c_str(), deleted ? "deleted" : "not found");
        preferences.end();
    }

    void ListPreferences()
    {
        println("Preferences List : %i", preferencesList.size());
        preferences.begin(PREF_NAMESPACE, true);
        for (size_t i = 0; i < preferencesList.size(); i++)
        {
            if(preferences.isKey(preferencesList[i].c_str()))
            {
                PreferenceType type = preferences.getType(preferencesList[i].c_str());
                if(type == PreferenceType::PT_I32)
                {
                    int32_t value = preferences.getInt(preferencesList[i].c_str());
                    println("  " + preferencesList[i] + " - Type: int32 = " + String(value));
                }
                else if(type == PreferenceType::PT_STR)
                {
                    String value = preferences.getString(preferencesList[i].c_str());
                    println("  " + preferencesList[i] + " - Type: String = " + value );
                }
                else
                println("  " + preferencesList[i] + " - Type: " + String((int)type));
            }
            else
            {
                println("  " + preferencesList[i] + " - Key not found.");
            }
        }
        preferences.end();
    }

    /*
    Base value is 500 entries
    keys storing values of type Bool, Char, UChar, Short, UShort, Int, UInt, Long, ULong, Long64, ULong64 use one entry in the key table.
    keys storing values of type Float and Double use three entries in the key table.
    Arduino or c-string String types use a minimum of two key table entries with the number of entries increasing with the length of the string.
    keys storing values of type Bytes use a minimum of three key table entries with the number of entries increasing with the number of bytes stored.
    A message providing the reason for a failed call is sent to the arduino-esp32 log_e facility.*/
    int32_t GetFreeEntries()
    {
        preferences.begin(PREF_NAMESPACE, true);
        int32_t freeEntries = preferences.freeEntries();
        println("Preferences Free Entries : %i", freeEntries);
        preferences.end();
        return freeEntries;
    }

    void ClearPreferences()
    {
        preferences.begin(PREF_NAMESPACE, false);
        preferences.clear();
        preferences.end();
        preferencesList.clear();
    }
    
    void HandleCommand(Command cmdTmp)
    {
        if(cmdTmp.cmd == "PrefList")
        {
            // PrefList
            Preferences_Helper::ListPreferences();
        }
        else if(cmdTmp.cmd == "PrefSave")
        {
            // PrefSave:cpt:123
            // PrefSave:str:data
            if (cmdTmp.dataStr1 != "" && cmdTmp.size == 1)
            {
                String key = cmdTmp.dataStr1;
                int32_t value = cmdTmp.data[0];
                println("Save preference : %s = %i", key.c_str(), value);
                Preferences_Helper::SaveToPreference(key, value);
            }
            else if (cmdTmp.dataStr1 != "" && cmdTmp.dataStr2 != "")
            {
                String key = cmdTmp.dataStr1;
                String value = cmdTmp.dataStr2;
                println("Save preference : %s = %s", key.c_str(), value.c_str());
                Preferences_Helper::SaveToPreference(key, value);
            }

        }
        else if(cmdTmp.cmd == "PrefLoad")
        {
            // PrefLoad:cpt:0
            // PrefLoad:str:default
            if (cmdTmp.dataStr1 != "" && cmdTmp.size == 1)
            {
                String key = cmdTmp.dataStr1;
                int32_t defValue = cmdTmp.data[0];
                int32_t value = Preferences_Helper::LoadFromPreference(key, defValue);
                println("Loaded preference : %s = %i", key.c_str(), value);
            }
            else if (cmdTmp.dataStr1 != "" && cmdTmp.dataStr2 != "")
            {
                String key = cmdTmp.dataStr1;
                String defValue = cmdTmp.dataStr2;
                String value = Preferences_Helper::LoadFromPreference(key, defValue);
                println("Loaded preference : %s = %s", key.c_str(), value.c_str());
            }
        }
        else if(cmdTmp.cmd == "PrefRemove")
        {
            // PrefRemove:cpt
            // PrefRemove:str
            if (cmdTmp.dataStr1 != "")
            {
                println("Remove preference : %s", cmdTmp.dataStr1.c_str());
                Preferences_Helper::RemoveFromPreference(cmdTmp.dataStr1);
            }
        }
        else if(cmdTmp.cmd == "PrefFree")
        {
            // PrefFree
            Preferences_Helper::GetFreeEntries();
        }
        else if (cmdTmp.cmd == "PrefClear")
        {
            // PrefClear:1
            if (cmdTmp.size == 1 && cmdTmp.data[0] == 1)
            {
                println("Clear all pref under: " + String(PREF_NAMESPACE));
                Preferences_Helper::ClearPreferences();
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
        println(" > PrefList");
        println("      List all key preferences");
        println(" > PrefSave:[string]:[int]");
        println("      Save the int value to preferences with the key");
        println(" > PrefSave:[string]:[string]");
        println("      Save the string value to preferences with the key");
        println(" > PrefLoad:[string]:[int]");
        println("      Load the int value from preferences with the key, return defValue if not found");
        println(" > PrefLoad:[string]:[string]");
        println("      Load the string value from preferences with the key, return defValue if not found");
        println(" > PrefRemove:[string]");
        println("      Remove the key from preferences");
        println(" > PrefFree");
        println("      Show free entries in preferences");
        println(" > PrefClear:[int]");
        println("      1 Clear all preferences under the namespace");
    }
}
