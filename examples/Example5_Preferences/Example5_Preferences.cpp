/*
    Start the serial communication at default baud rate speed : 921600
    SERIAL_DEBUG is the Serial port used for communication with the PC
    You can redefine it to change the Serial port used
*/

#include "ESP32_Helper.h"
using namespace Printer;

int32_t cpt;
void setup(void)
{
    ESP32_Helper::Initialisation();
    Preferences_Helper::ListPreferences();
    Preferences_Helper::GetFreeEntries();
    cpt = Preferences_Helper::LoadFromPreference("cpt", -1);
    println("Load cpt = %i", cpt);
}

void loop(void)
{
    cpt = Preferences_Helper::LoadFromPreference("cpt", cpt);
    // This will be print on Serial
    println("Hello World %i times !", cpt++);
    Preferences_Helper::SaveToPreference("cpt", cpt);
    delay(5000);
}
