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
            
            Printer::println("Debugger: " + status);
            debuggerEnable = enable;
        }
    }

    bool IsEnable() { return debuggerEnable == Enable::ENABLE_TRUE; }

    void Initialisation()
    {
        Printer::print("Preparing queueSteps : ");
        queueSteps = QueueThread<int16_t>(100);
        if (!queueSteps.IsInit())
        {
            Printer::println("Error creating the queueSteps !");
        }
        Printer::println("done.");
    }

    void HandleCommand(Command cmdTmp)
    {
         if (cmdTmp.cmd == "DebugEnable")
        {
            // DebugEnable:1
            if (cmdTmp.size == 1)
                EnableDebugger((Enable)cmdTmp.data.at(0));
        }
        else if (cmdTmp.cmd == "DebugAddSteps")
        {
            // DebugAddSteps:10
            if (cmdTmp.size == 1 && cmdTmp.data.at(0) > 0)
                AddSteps(cmdTmp.data.at(0));
        }
        else if (cmdTmp.cmd == "DebugGetSteps")
        {
            // DebugGetSteps
            Printer::println("Steps available : %i", GetSteps());
        }
        else
        {
            Printer::println("Not a Debug command !");
            PrintCommandHelp();
        }
    }

    void PrintCommandHelp()
    {
        Printer::println("Debugger Command Help :");
        Printer::println(" > DebugEnable:[int]");
        Printer::println("      0 Disable, 1 Enable Debugger");
        Printer::println(" > DebugAddSteps:[int]");
        Printer::println("      Add the number of steps to the debugger");
        Printer::println(" > DebugGetSteps");
        Printer::println("      Get the actual number of steps available");
        Printer::println();
    }

    bool WaitForAvailableSteps()
    {
        if (IsEnable())
        {
            ReceiveSteps();

            if (debugSteps <= 0)
                Printer::println("Debugger : Wait For Available Steps");

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
