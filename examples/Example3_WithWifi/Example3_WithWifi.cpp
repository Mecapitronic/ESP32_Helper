/*
    Start the serial communication at default baud rate speed : 921600
*/
#ifndef WITH_WIFI
    #define WITH_WIFI
#endif

#include "ESP32_Helper.h"
using namespace Printer;


void setup(void)
{
    ESP32_Helper::Initialisation();
}

static int32_t cpt = 0;
void loop(void)
{
    // This will be send over Wifi, not on Serial
    println("Hello World ", cpt++, " times !");
    delay(5000);
}
