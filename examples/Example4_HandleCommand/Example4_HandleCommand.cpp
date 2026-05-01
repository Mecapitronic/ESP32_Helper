/*
    Start the serial communication at default baud rate speed : 921600
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

// Custom handler for counter commands
void handleCounterCommand(Command cmd)
{
    if(cmd.cmd.startsWith("cpt++"))
    {
        cpt++;
        println("Counter incremented: %i", cpt);
    }
    else if(cmd.cmd.startsWith("cpt--"))
    {
        cpt--;
        println("Counter decremented: %i", cpt);
    }
    else if(cmd.cmd == "cpt" && cmd.size == 1)
    {
        cpt = cmd.data[0];
        println("Counter set to: %i", cpt);
    }
}

// Custom handler for log commands
void handleLogCommand(Command cmd)
{
    if(cmd.cmd.startsWith("Log"))
    {
        println("Log : %s", cmd.ToString().c_str());
    }
}

// ============== Setup ==============

void setup(void)
{
    ESP32_Helper::Initialisation();
    
    // Register custom command handlers
    // Commands starting with "cpt" will be handled by handleCounterCommand
    ESP32_Helper::RegisterCommandHandler("cpt", handleCounterCommand);
    
    // Commands starting with "Log" will be handled by handleLogCommand
    ESP32_Helper::RegisterCommandHandler("Log", handleLogCommand);
    
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
    // 
    // Unhandled commands will still be available via GetCommand()
    if (ESP32_Helper::HasWaitingCommand())
    {
        Command command = ESP32_Helper::GetCommand();
        println("Unhandled command received: %s", command.ToString().c_str());
    }

    // Try sending "Help" to see the list of commands available from the Lib
}
