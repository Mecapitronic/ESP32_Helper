/**
 * @file Preferences_Helper.h
 * @author Mecapitronic (mecapitronic@gmail.com)
 * @brief Used to saved and load data from preferences
 * @date 2023-07-25
 */

#ifndef PREFERENCES_HELPER_H
#define PREFERENCES_HELPER_H

#include <Preferences.h>
#include "ESP32_Helper.h"

namespace Preferences_Helper
{
    int LoadFromPreference(String pref, int defValue = 0);
    String LoadFromPreference(String pref, String defValue = "");
    void SaveToPreference(String pref, int value);
    void SaveToPreference(String pref, String value);
}

#endif