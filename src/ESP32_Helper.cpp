#include "ESP32_Helper.h"

using namespace Printer;

namespace ESP32_Helper
{
    namespace // anonymous nested namespace, cannot access outside this file
    {
        TaskThread taskUpdate;
        QueueThread<Command> awaitingCommand;

        // converts character array
        // to string and returns it
        String convertToString(char *a, int size)
        {
            String s = "";
            for (int i = 0; i < size; i++)
            {
                s = s + a[i];
            }
            return s;
        }
    }

    void Initialisation(BaudRate baud_speed, Enable printEnable, Level printLvl, Enable debugEnable)
    {
        Serial.end();
        Serial.setRxBufferSize(1024);
        Serial.setTxBufferSize(1024);
        Serial.begin(static_cast<ulong>(baud_speed));
        Serial.println();
        Serial.println("-- Starting Helper Initialisation --");
        Wifi_Helper::Initialisation();
        Serial.println("Creating Incoming Command Queue");
        awaitingCommand = QueueThread<Command>(20);
        if (!awaitingCommand.IsInit())
        {
            Serial.println("Error creating the queue : awaitingCommand");
        }

        Serial.println("Creating Incoming Command Update Task");
        /* Task function. */
        /* name of task. */
        /* Stack size of task */
        /* parameter of the task */
        /* priority of the task */
        /* Task handle to keep track of created task */
        /* pin task to core 0 */
        taskUpdate = TaskThread(ESP32_Helper::Update, "CommandUpdate");

        Serial.println("-- Starting Printer Initialisation --");
        Printer::Initialisation();
        Printer::SetLevel(printLvl);
        Printer::EnablePrinter(printEnable);
        Serial.println("-- End of Printer Initialisation --");

        println();
        println("+---------------+");
        println("¦  M E C A P I  ¦");
        println("+---------------+");
        println();
        print(__DATE__);
        print(" at ");
        println(__TIME__);
        println();

        Serial.println("-- Starting Debugger Initialisation --");
        Debugger::Initialisation();
        Debugger::EnableDebugger(debugEnable);
        Serial.println("-- End of Debugger Initialisation --");

        Serial.println("-- End of Helper Initialisation --");
        Serial.println();
    }

    void Update(void *pvParameters)
    {
        const int readBufferMax = 64;
        vector<char> readBuffer;
        readBuffer.reserve(readBufferMax);

        for (;;)
        {
            while (SERIAL_DEBUG.available() > 0)
            {
                char tmpChar = SERIAL_DEBUG.read();
                if (readBuffer.size() < readBuffer.capacity())
                {
                    readBuffer.push_back(tmpChar);
                    if (tmpChar == '\n')
                    {
                        SERIAL_DEBUG.print("Received ");
                        SERIAL_DEBUG.print(readBuffer.size());
                        SERIAL_DEBUG.print(" : ");
                        SERIAL_DEBUG.write(readBuffer.data(), readBuffer.size());
                        SERIAL_DEBUG.println();
                        BufferReadCommand(readBuffer.data(), readBuffer.size()); // pass data and size
                        readBuffer.clear();
                    }
                }
                else
                {
                    SERIAL_DEBUG.print("Read Buffer Overflow : ");
                    SERIAL_DEBUG.println(readBuffer.size());
                    readBuffer.clear();
                }
            }
            vTaskDelay(1);
        }
        Printer::println("Command Update Task STOPPED !");
    }

    void HandleCommand(Command cmdTmp)
    {

    }
    
    void BufferReadCommand(char *read, int size)
    {
        Command cmdTmp;
        uint16_t indexSeparator = 0;
        // Start at 1 to let one letter for command at least
        for (uint8_t i = 1; i < size; i++)
        {
            if (cmdTmp.cmd == "" && (read[i] == ':' || read[i] == '\n'))
            {
                cmdTmp.cmd = String(&read[0], i);
                indexSeparator = i + 1;
            }
            else if (cmdTmp.cmd != "" && (read[i] == ';' || read[i] == ':' || read[i] == '\n'))
            {
                //cmdTmp.data[cmdTmp.size++] = atoi(String(&read[indexSeparator], i-indexSeparator).c_str());
                //check if it's an int or a string by trying to convert it
                String strToConvert = String(&read[indexSeparator], i-indexSeparator);
                try
                {
                    std::size_t pos{};
                    int conversion = std::stoi(strToConvert.c_str(), &pos);

                    if (pos == i - indexSeparator)
                    {
                        cmdTmp.data[cmdTmp.size++] = conversion;
                    }
                    else
                    {
                        //println("Conversion didn't consume entire string.");
                        if(i - indexSeparator <= Command::sizeStr)
                            cmdTmp.dataStr = strToConvert;                        
                        //else
                        //   println("String too long !");
                    }
                }
                catch(const std::exception& e)
                {
                    //println("Conversion error: not an integer");                    
                    if(i - indexSeparator <= Command::sizeStr)
                        cmdTmp.dataStr = strToConvert;
                    //else
                    //    println("String too long !");
                }
                indexSeparator = i + 1;
            }
        }
        Printer::println("Command Received : ", cmdTmp);

        if(cmdTmp.cmd != "")
        {
            if (cmdTmp.cmd.startsWith("Help"))
            {
                Printer::println("Help Commands : ");
                Printer::PrintCommandHelp();
                Preferences_Helper::PrintCommandHelp();
                Debugger::PrintCommandHelp();
                Wifi_Helper::PrintCommandHelp();
            }
            if (cmdTmp.cmd.startsWith("Reboot"))
            {
                Printer::println("Rebooting...");
                ESP.restart();
            }
            // We first handle if the command is for the Lib
            else if (cmdTmp.cmd.startsWith("Debug"))
                Debugger::HandleCommand(cmdTmp);
            else if (cmdTmp.cmd.startsWith("Print"))
                Printer::HandleCommand(cmdTmp);
            else if (cmdTmp.cmd.startsWith("Preferences"))
                Printer::HandleCommand(cmdTmp);
            else if (cmdTmp.cmd.startsWith("Wifi"))
                Wifi_Helper::HandleCommand(cmdTmp);
            else
            {
                // If command is not for Lib, we send it to the main
                awaitingCommand.Send(cmdTmp);
            }
        }
    }

    bool HasWaitingCommand() { return awaitingCommand.MessagesWaiting() > 0; }

    Command GetCommand()
    {
        Command cmd;
        if (HasWaitingCommand())
        {
            if (awaitingCommand.Receive(cmd))
            {
                return cmd;
            }
        }
        return cmd;
    }
}
