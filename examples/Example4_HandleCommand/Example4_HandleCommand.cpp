/*
    Start the serial communication at default baud rate speed : 921600
    SERIAL_DEBUG is the Serial port used for communication with the PC
    You can redefine it to change the Serial port used
*/

#include "ESP32_Helper.h"
using namespace Printer;

void setup(void)
{
    ESP32_Helper::Initialisation();
}

static int32_t cpt = 0;
void loop(void)
{    
    // This will be print on Serial
    println("Hello World ", cpt, " times !");
    delay(1000);

    // Command is defined with a string first (14 char max),
    // then separator for each data ';' or ':' (6 data max),
    // optionnal string whereever between data (14 char max)
    if (ESP32_Helper::HasWaitingCommand())
    {
        Command command = ESP32_Helper::GetCommand();
        // cpt++
        if(command.cmd.startsWith("cpt++"))
        {
            cpt++;
        }
        // cpt--
        if(command.cmd.startsWith("cpt--"))
        {
            cpt--;
        }
        // cpt:5
        if(command.cmd == "cpt" && command.size == 1)
        {
            cpt = command.data[0];
        }
        // Log;Message here
        if(command.cmd.startsWith("Log"))
        {
            println("Log : ", command.dataStr1);
        }
        // put your command here
    }

    // Try sending "Help" to see the list of commands available from the Lib
}
