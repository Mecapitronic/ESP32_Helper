#include "ESP32_Helper.h"

using namespace Printer;
using namespace std;

namespace ESP32_Helper
{
    namespace // anonymous nested namespace, cannot access outside this file
    {
        TaskThread taskUpdate;
        QueueThread<Command> awaitingCommand;
    }

    void Initialisation(BaudRate baud_speed, Enable printEnable, Level printLvl, Enable debugEnable)
    {
        Serial.end();
        Serial.setRxBufferSize(1024);
        Serial.setTxBufferSize(1024);
        Serial.begin(static_cast<unsigned long>(baud_speed));
        delay(2000);
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
        const int8_t readBufferMax = 64;
        vector<char> readBuffer;
        readBuffer.reserve(readBufferMax);

        for (;;)
        {
            while (SERIAL_DEBUG.available() > 0)
            {
                char tmpChar = SERIAL_DEBUG.read();
                if (readBuffer.size() < readBuffer.capacity())
                {
                    if(tmpChar == '\r')
                        continue; // Ignore Carriage Return
                    readBuffer.push_back(tmpChar);
                    if (tmpChar == '\n')
                    {
                        if(readBuffer.size() > 1)
                        {
                            SERIAL_DEBUG.print("Received ");
                            SERIAL_DEBUG.print(readBuffer.size());
                            SERIAL_DEBUG.print(" : ");
                            SERIAL_DEBUG.write(readBuffer.data(), readBuffer.size()-1);
                            SERIAL_DEBUG.println();
                            BufferReadCommand(readBuffer.data(), readBuffer.size()); // pass data and size
                        }
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
        if(cmdTmp.cmd != "")
        {
            if (cmdTmp.cmd.startsWith("Help"))
            {
                Printer::println("Help Commands : ");
                Debugger::PrintCommandHelp();
                Printer::PrintCommandHelp();
                Preferences_Helper::PrintCommandHelp();
                Wifi_Helper::PrintCommandHelp();
            }
            else if (cmdTmp.cmd.startsWith("Reboot"))
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
                Preferences_Helper::HandleCommand(cmdTmp);
            else if (cmdTmp.cmd.startsWith("Wifi"))
                Wifi_Helper::HandleCommand(cmdTmp);
            else
            {
                // If command is not for Lib, we send it to the main
                awaitingCommand.Send(cmdTmp);
            }
        }
    }

    void BufferReadCommand(char *read, int32_t size)
    {
        Command cmdTmp;
        uint16_t indexSeparator = 0;
        bool isString = false;
        // Start at 1 to let one letter for command at least
        for (int32_t i = 1; i < size; i++)
        {
            if(read[i] != ';' && read[i] != ':' && read[i] != '\n')
            {
                if(read[i] < 0x30 || read[i] > 0x39)
                    isString = true;
            }
            else
            {        
                if (cmdTmp.cmd == "")
                {
                    cmdTmp.cmd = String(&read[0], i);
                    indexSeparator = i + 1;
                }
                else
                {
                    if(i - indexSeparator > 0)
                    {
                        // check if it's a string, if it's an integer : convert it
                        String strToConvert = String(&read[indexSeparator], i - indexSeparator);
                        if(isString)
                        {
                            if(i - indexSeparator <= Command::sizeStr)
                                    cmdTmp.dataStr = strToConvert;
                            //else
                                //println("String too long !");
                        }
                        else
                        {
                            std::size_t pos{};
                            try
                            {
                                int32_t conversion = std::stoi(strToConvert.c_str(), &pos);
                                if (pos == i - indexSeparator)
                                {
                                    cmdTmp.data[cmdTmp.size++] = conversion;
                                }
                                else
                                {
                                    //println("Conversion didn't consume entire string.");
                                }
                            }
                            catch(const std::exception& e)
                            {
                                println(e.what(), String(" error : "), strToConvert, Level::LEVEL_ERROR);
                            }
                        }
                        isString = false;
                    }
                    indexSeparator = i + 1;
                }
            }
        }
        Printer::println("Command Received : ", cmdTmp);
        HandleCommand(cmdTmp);
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
