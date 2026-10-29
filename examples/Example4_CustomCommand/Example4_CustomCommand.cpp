/*
    Start the serial communication at default baud rate speed 921600
    SERIAL_DEBUG is the Serial port used for communication with the PC
    You can redefine it to change the Serial port used
    
    This example also demonstrates how to register custom command
    using RegisterCommand() for extensible command processing.
*/

#include "ESP32_Helper.h"
using namespace Printer;

// ============== Global Variables ==============

int32_t cpt = 0;

// ============== Setup ==============

void setup(void)
{
    ESP32_Helper::Initialisation();
    
    // Register custom command handlers for counter and log commands : Command Name, Help description, FunctionCommand to be executed

    ESP32_Helper::RegisterCommand("cpt++", "cpt++ - Increment the counter.", FunctionCommand( cpt++; println("Counter incremented %i", cpt); ));
    ESP32_Helper::RegisterCommand("cpt--", "cpt-- - Decrement the counter.", FunctionCommand( cpt--; println("Counter decremented %i", cpt); ));
    ESP32_Helper::RegisterCommand("cpt", "cpt:<value:int> - Set the counter. Example: cpt:5", FunctionCommand( if(cmd.hasData()){ cpt = cmd.data[0]; println("Counter set to %i", cpt); } ));

    ESP32_Helper::RegisterCommand("Log", "Log:<message:string> - Print a log message. Example: Log:MyMessage", FunctionCommand( if(cmd.hasDataStr1()) { println("Log %s", cmd.dataStr1); } ));
    
    println("Custom handlers registered!");

    ESP32_Helper::HandleCommand("Help");
}

// ============== Loop ==============

void loop(void)
{    
    // This will be print on Serial
    println("Hello World %i times !", cpt++);
    delay(1000);

    // Commands are now handled by registered custom handlers
    // Try sending:
    //   "cpt++" to increment counter
    //   "cpt--" to decrement counter
    //   "cpt:5" to set counter to 5
    //   "Log:MyMessage" to log a message

    // Try sending "Help" to see the list of commands available from the Lib
}