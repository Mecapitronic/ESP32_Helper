/*
    Start the serial communication at default baud rate speed 921600
    SERIAL_DEBUG is the Serial port used for communication with the PC
    You can redefine it to change the Serial port used
    
    This example also demonstrates how to register custom command handlers
    using RegisterCommandHandler() for extensible command processing.
*/

#include "ESP32_Helper.h"
using namespace Printer;

// ============== Global Variables ==============

int32_t cpt = 0;

// ============== Custom Command Handlers ==============

// Help function for counter commands
void printCounterHelp()
{
    println("Counter Commands");
    println(" > cpt++");
    println("     Increment counter");
    println(" > cpt--");
    println("     Decrement counter");
    println(" > cpt:value");
    println("     Set counter to value");
}

// Custom handler for counter commands
// Returns true if the command was handled, false otherwise
bool handleCounterCommand(Command cmd)
{
    if(cmd.cmdStartsWith("cpt++"))
    {
        cpt++;
        println("Counter incremented %i", cpt);
        return true;
    }
    else if(cmd.cmdStartsWith("cpt--"))
    {
        cpt--;
        println("Counter decremented %i", cpt);
        return true;
    }
    else if(cmd.cmdEquals("cpt") && cmd.size == 1)
    {
        cpt = cmd.data[0];
        println("Counter set to %i", cpt);
        return true;
    }
    return false;
}

// Help function for log commands
void printLogHelp()
{
    println("Log Commands");
    println(" > Log:message");
    println("     Print a log message");
}

// Custom handler for log commands
// Returns true if the command was handled, false otherwise
bool handleLogCommand(Command cmd)
{
    if(cmd.cmdStartsWith("Log"))
    {
        println("Log %s", cmd.ToString().c_str());
        return true;
    }
    return false;
}

// ============== Setup ==============

void setup(void)
{
    ESP32_Helper::Initialisation();
    
    ESP32_Helper::RegisterCommand("cpt++", "Increment counter", [](Command cmd){ cpt++; println("Counter incremented %i", cpt); });
    ESP32_Helper::RegisterCommand("cpt--", "Decrement counter", [](Command cmd){ cpt--; println("Counter decremented %i", cpt); });
    ESP32_Helper::RegisterCommand("cpt", "Set counter to value, cpt:0", [](Command cmd){ if(cmd.size == 1){ cpt = cmd.data[0]; println("Counter set to %i", cpt); } });


    ESP32_Helper::RegisterCommand("Log", "Print a log message, Log:message", [](Command cmd){ println("Log %s", cmd.ToString().c_str()); });
    
    println("Custom handlers registered!");
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
