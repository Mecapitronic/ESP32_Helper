#include "Debugger.h"

namespace Debugger
{
    namespace
    {
        QueueThread<int16_t> queueSteps;
        Enable debuggerEnable = Enable::ENABLE_NONE;
        int32_t debugSteps = 0;
    }

    void EnableDebugger(Enable enable)
    {
        if (enable != Enable::ENABLE_NONE)
        {
            String status = "";
            if (debuggerEnable == enable && enable == Enable::ENABLE_TRUE)
                status = "already Enabled";
            else if (debuggerEnable == enable && enable == Enable::ENABLE_FALSE)
                status = "already Disabled";
            else if (debuggerEnable != enable && enable == Enable::ENABLE_TRUE)
                status = "Enabled";
            else if (debuggerEnable != enable && enable == Enable::ENABLE_FALSE)
                status = "Disabled";
            
            Printer::println("Debugger " + status);
            debuggerEnable = enable;
        }
    }

    bool IsEnable() { return debuggerEnable == Enable::ENABLE_TRUE; }

    void Initialisation()
    {
        queueSteps = QueueThread<int16_t>(100);
        if (!queueSteps.IsInit())
        {
            Printer::println("Preparing queueSteps Error creating the queueSteps !");
            return;
        }
        Printer::println("Preparing queueSteps done.");
        
        RegisterCommands();
    }

    void RegisterCommands()
    {
        ESP32_Helper::RegisterCommand("DebugEnable", "DebugEnable:<enabled:bool> - Enable or disable the debugger.", FunctionCommand( if(cmd.hasData()) EnableDebugger((Enable)cmd.data[0]); ));
        ESP32_Helper::RegisterCommand("DebugAddSteps", "DebugAddSteps:<steps:int> - Add debugger steps", FunctionCommand( if(cmd.hasData() && cmd.data[0] > 0) AddSteps(cmd.data[0]); ));
        ESP32_Helper::RegisterCommand("DebugGetSteps", "DebugGetSteps - Show the number of available debugger steps.", FunctionCommand( Printer::println("Steps available %i", GetSteps()); ));
    }

    bool WaitForAvailableSteps()
    {
        if (IsEnable())
        {
            ReceiveSteps();

            if (debugSteps <= 0)
                Printer::println("Debugger Wait For Available Steps");

            while (debugSteps <= 0 && IsEnable())
            {
                ReceiveSteps();
            }
            debugSteps--;
        }
        else if (debugSteps != 0)
            debugSteps = 0;
        return true;
    }

    void AddSteps(int16_t steps)
    {
        Printer::println("Adding %d steps to Debugger.", steps);
        queueSteps.Send(steps);
    }

    void ReceiveSteps()
    {
        if (queueSteps.MessagesWaiting() > 0)
        {
            int16_t steps = 0;
            if (queueSteps.Receive(steps))
            {
                debugSteps += steps;
            }
        }
    }

    int GetSteps()
    {
        return debugSteps;
    }

}
