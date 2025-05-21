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
    cpt = Preferences_Helper::LoadFromPreference("cpt", -1);
    println("Load cpt = ", cpt);
}

void loop(void)
{
    // This will be print on Serial
    println("Hello World ", cpt, " times !");
    delay(1000);

    Preferences_Helper::SaveToPreference("cpt", cpt);
    println("Saved cpt = ", cpt);
}
