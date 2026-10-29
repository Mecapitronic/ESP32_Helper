#include "Preferences_Helper.h"

using namespace Printer;
using namespace std;

namespace Preferences_Helper
{
    Preferences preferences;
    // This list is not perfect because if you save a key manually by command and reboot, it won't be in the list
    vector<String> preferencesList;
    const char * const PREF_NAMESPACE = "Mecapi";

    void Initialisation()
    {
        // First boot can fail in read-only mode if the namespace does not exist yet.
        if (preferences.begin(PREF_NAMESPACE))
        {
            preferences.end();
        }
        RegisterCommands();
    }
    
    void RegisterCommands()
    {
        ESP32_Helper::RegisterCommand("PrefList", "PrefList - List known preference keys and their values.", FunctionCommand( Preferences_Helper::ListPreferences(); ));
        ESP32_Helper::RegisterCommand("PrefSave", "PrefSave:<key:string>:<value:int|string> - Save an integer or string preference.", FunctionCommand(
            if(cmd.hasDataStr1() && cmd.hasData()){ Preferences_Helper::SaveToPreference(String(cmd.dataStr1), cmd.data[0]); }
            else if(cmd.hasDataStr1() && cmd.hasDataStr2()){ Preferences_Helper::SaveToPreference(String(cmd.dataStr1), String(cmd.dataStr2)); }
        ));
        ESP32_Helper::RegisterCommand("PrefShow", "PrefShow:<key:string>:<defaultValue:int|string> - Read a preference; use defaultValue if missing. Its type selects the stored type.", FunctionCommand(
            if(cmd.hasDataStr1() && cmd.hasData()) Preferences_Helper::LoadFromPreference(String(cmd.dataStr1), cmd.data[0]);
            else if(cmd.hasDataStr1() && cmd.hasDataStr2()) Preferences_Helper::LoadFromPreference(String(cmd.dataStr1), String(cmd.dataStr2));
        ));
        ESP32_Helper::RegisterCommand("PrefRemove", "PrefRemove:<key:string> - Remove a preference key.", FunctionCommand( if(cmd.hasDataStr1()) Preferences_Helper::RemoveFromPreference(String(cmd.dataStr1)); ));
        ESP32_Helper::RegisterCommand("PrefShowFree", "PrefShowFree - Show free preference entries.", FunctionCommand( Preferences_Helper::GetFreeEntries(); ));
        ESP32_Helper::RegisterCommand("PrefClearAll", "PrefClearAll - Delete all preferences in the current namespace.", FunctionCommand( Preferences_Helper::ClearPreferences(); ));
    }

    int32_t LoadFromPreference(const String &pref, int32_t defValue)
    {
        preferences.begin(PREF_NAMESPACE, true);
        int32_t value = preferences.getInt(pref.c_str(), defValue);
        if(preferencesList.size() == 0 || std::find(preferencesList.begin(), preferencesList.end(), pref) == preferencesList.end())
            preferencesList.push_back(pref);
        println("Load Preferences " + pref + " = " + String(value));
        preferences.end();
        return value;
    }

    String LoadFromPreference(const String &pref, const String &defValue)
    {
        preferences.begin(PREF_NAMESPACE, true);
        String value = preferences.getString(pref.c_str(), defValue.c_str());
        if(preferencesList.size() == 0 || std::find(preferencesList.begin(), preferencesList.end(), pref) == preferencesList.end())
            preferencesList.push_back(pref);
        println("Load Preferences " + pref + " = " + value);
        preferences.end();
        return value;
    }

    void SaveToPreference(const String &pref, int32_t value)
    {
        preferences.begin(PREF_NAMESPACE, false);
        preferences.putInt(pref.c_str(), value);
        if(preferencesList.size() == 0 || std::find(preferencesList.begin(), preferencesList.end(), pref) == preferencesList.end())
            preferencesList.push_back(pref);
        println("Save Preferences " + pref + " = " + value);
        preferences.end();
    }

    void SaveToPreference(const String &pref, const String &value)
    {
        preferences.begin(PREF_NAMESPACE, false);
        preferences.putString(pref.c_str(), value.c_str());
        if(preferencesList.size() == 0 || std::find(preferencesList.begin(), preferencesList.end(), pref) == preferencesList.end())
            preferencesList.push_back(pref);
        println("Save Preferences " + pref + " = " + value);
        preferences.end();
    }

    void RemoveFromPreference(const String &pref)
    {
        preferences.begin(PREF_NAMESPACE, false);
        bool deleted = preferences.remove(pref.c_str());
        auto it = std::find(preferencesList.begin(), preferencesList.end(), pref);
        if(it != preferencesList.end())
            preferencesList.erase(it);
        println("Remove Preferences %s %s", pref.c_str(), deleted ? "deleted" : "not found");
        preferences.end();
    }

    void ListPreferences()
    {
        println("Preferences List %i", preferencesList.size());
        preferences.begin(PREF_NAMESPACE, true);
        for (size_t i = 0; i < preferencesList.size(); i++)
        {
            if(preferences.isKey(preferencesList[i].c_str()))
            {
                PreferenceType type = preferences.getType(preferencesList[i].c_str());
                if(type == PreferenceType::PT_I32)
                {
                    int32_t value = preferences.getInt(preferencesList[i].c_str());
                    println("  " + preferencesList[i] + " - Type int32 = " + String(value));
                }
                else if(type == PreferenceType::PT_STR)
                {
                    String value = preferences.getString(preferencesList[i].c_str());
                    println("  " + preferencesList[i] + " - Type String = " + value );
                }
                else
                println("  " + preferencesList[i] + " - Type " + String((int)type));
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
        println("Preferences Free Entries %i", freeEntries);
        preferences.end();
        return freeEntries;
    }

    void ClearPreferences()
    {
        preferences.begin(PREF_NAMESPACE, false);
        preferences.clear();
        preferences.end();
        preferencesList.clear();
        println("Preferences cleared for namespace " + String(PREF_NAMESPACE));
    }
}
