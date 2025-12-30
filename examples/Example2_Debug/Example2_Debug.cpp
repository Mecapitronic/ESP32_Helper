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
    Debugger::EnableDebugger(Enable::ENABLE_TRUE);
    Debugger::AddSteps(10);
}

int32_t cpt = 0;
void loop(void)
{
    // Send
    // DebugSteps:10/n
    // over Serial to unblock the loop
    Debugger::WaitForAvailableSteps();
    println("Hello World %i times !", cpt++);
    delay(1000);
}
