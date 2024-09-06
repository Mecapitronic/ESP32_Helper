#include "Preferences_Helper.h"

namespace Preferences_Helper
{
    Preferences preferences;

    int LoadFromPreference(String pref, int defValue)
    {
        preferences.begin("Mecapi", true);
        int value = preferences.getInt(pref.c_str(), defValue);
        Printer::print("Preferences ");
        Printer::println(pref, value);
        preferences.end();
        return value;
    }

    String LoadFromPreference(String pref, String defValue)
    {
        preferences.begin("Mecapi", true);
        String value = preferences.getString(pref.c_str(), defValue.c_str());
        Printer::print("Preferences ");
        Printer::println(pref, "=", value);
        preferences.end();
        return value;
    }
    /*
    uint8_t* LoadFromPreference(String pref, void* defValue, int length)
    {
        preferences.begin("Mecapi", true);
        uint8_t* value = preferences.getBytes(pref.c_str(), defValue, length);
        Printer::print("Preferences ");
        Printer::println(pref, "=", value);
        preferences.end();
        return value;
    }*/

    void SaveToPreference(String pref, int value)
    {
        preferences.begin("Mecapi", false);
        preferences.putInt(pref.c_str(), value);
        preferences.end();
    }

    void SaveToPreference(String pref, String value)
    {
        preferences.begin("Mecapi", false);
        preferences.putString(pref.c_str(), value.c_str());
        preferences.end();
    }
}
