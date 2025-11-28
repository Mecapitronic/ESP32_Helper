/**
 * @file Preferences_Helper.h
 * @author Mecapitronic (mecapitronic@gmail.com)
 * @brief Used to saved and load data from preferences
 * @date 2023-07-25
 */

#ifndef PREFERENCES_HELPER_H
#define PREFERENCES_HELPER_H

#include "Preferences.h"
#include "ESP32_Helper.h"

namespace Preferences_Helper
{
    int32_t LoadFromPreference(const String &pref, int32_t defValue = 0);
    String LoadFromPreference(const String &pref, const String &defValue = "");
    void SaveToPreference(const String &pref, int32_t value);
    void SaveToPreference(const String &pref, const String &value);
    void ClearPreferences();
    void HandleCommand(Command cmdTmp);
    void PrintCommandHelp(void);
}

#endif
