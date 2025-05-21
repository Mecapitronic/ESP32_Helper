/*
    Start the serial communication at default baud rate speed : 921600
    SERIAL_DEBUG is the Serial port used for communication with the PC
    You can redefine it to change the Serial port used
*/

#ifndef WITH_WIFI
    #define WITH_WIFI
#endif
#ifndef OTA
    #define OTA
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
    // This will be send over Wifi if enable and also on Serial
    println("Hello World ", cpt++, " times !");
    delay(1000);

    // you can Upload with platformio Over The Air on the IP address

    // Send "WifiStatus" command to see all wifi parameters
    // You can change the Wifi parameters with commands
    // All this parameters will be saved into the ESP32 flash memory and reload at startup
}
