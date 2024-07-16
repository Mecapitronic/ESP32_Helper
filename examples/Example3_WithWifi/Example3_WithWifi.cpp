/*
    Start the serial communication at default baud rate speed : 921600

*/

#include "ESP32_Helper.h"
using namespace Printer;

void setup(void)
{
    ESP32_Helper::ESP32_Helper();
}

void loop(void)
{
    ESP32_Helper::UpdateSerial();
}