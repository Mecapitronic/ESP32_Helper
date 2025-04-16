/*
    Start the serial communication at default baud rate speed : 921600
*/

#include "ESP32_Helper.h"
using namespace Printer;

void setup(void)
{
    ESP32_Helper::Initialisation();
    Debugger::EnableDebugger(Enable::ENABLE_TRUE);
    Debugger::AddSteps(10);
}

static int32_t cpt = 0;
void loop(void)
{
    // Send
    // DebugSteps:10/n
    // over Serial to unblock the loop
    Debugger::WaitForAvailableSteps();
    println("Hello World ", cpt++, " times !");
    delay(1000);
}
