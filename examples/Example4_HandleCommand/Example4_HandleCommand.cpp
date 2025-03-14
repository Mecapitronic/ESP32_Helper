/*
    Start the serial communication at default baud rate speed : 921600
*/

#include "ESP32_Helper.h"
using namespace Printer;

void setup(void)
{
    ESP32_Helper::Initialisation();
}

int32_t cpt = 0;
void loop(void)
{    
    // This will be print on Serial
    println("Hello World ", cpt, " times !");
    delay(1000);

    if (ESP32_Helper::HasWaitingCommand())
    {
        Command command = ESP32_Helper::GetCommand();
        if(command.cmd.startsWith("cpt++"))
        {
            cpt++;
        }
    }
}
