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
            Printer::print("Debugger : ");

            if (debuggerEnable == enable && enable == Enable::ENABLE_TRUE)
            {
                Printer::print("already Enable");
            }
            if (debuggerEnable == enable && enable == Enable::ENABLE_FALSE)
            {
                Printer::print("already Disable");
            }
            if (debuggerEnable != enable && enable == Enable::ENABLE_TRUE)
            {
                Printer::print(" Enable");
            }
            if (debuggerEnable != enable && enable == Enable::ENABLE_FALSE)
            {
                Printer::print(" Disable");
            }
            Printer::println();
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
        if (cmdTmp.cmd == "DebugSteps")
        {
            // DebugSteps:10
            if (cmdTmp.size == 1 && cmdTmp.data.at(0) > 0)
                Debugger::AddSteps(cmdTmp.data.at(0));
        }
        else if (cmdTmp.cmd == "DebugEnable")
        {
            // DebugEnable:1
            if (cmdTmp.size == 1)
                Debugger::EnableDebugger((Enable)cmdTmp.data.at(0));
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
        Printer::println(" > DebugSteps:[int]");
        Printer::println("      Add the number of steps to the debugger");
        Printer::println(" > DebugEnable:[int]");
        Printer::println("      0 Disable, 1 Enable Debugger");
        Printer::println();
    }

    bool WaitForAvailableSteps()
    {
        if (IsEnable())
        {
            GetSteps();

            if (debugSteps <= 0)
                Printer::println("Debugger : Wait For Available Steps");

            while (debugSteps <= 0 && IsEnable())
            {
                GetSteps();
            }
            debugSteps--;
        }
        else if (debugSteps != 0)
            debugSteps = 0;
        return true;
    }

    void AddSteps(int16_t steps)
    {
        queueSteps.Send(steps);
        Printer::println("Adding ", steps, " steps to Debugger.");
    }

    bool GetSteps()
    {
        if (queueSteps.MessagesWaiting() > 0)
        {
            int16_t steps = 0;
            if (queueSteps.Receive(steps))
            {
                debugSteps += steps;
                return true;
            }
        }
        return false;
    }

}
