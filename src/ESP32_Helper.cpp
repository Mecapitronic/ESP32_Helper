#include "ESP32_Helper.h"

using namespace Printer;
using namespace std;

namespace ESP32_Helper
{
    namespace // anonymous nested namespace, cannot access outside this file
    {
        const int8_t readBufferMax = 64;
        std::vector<char> readBuffer;
        TaskThread taskUpdate;
        QueueThread<Command> awaitingCommand;
    }

    void Initialisation(BaudRate baud_speed, Enable printEnable, Level printLvl, Enable debugEnable)
    {
        SERIAL_DEBUG.end();
        SERIAL_DEBUG.setRxBufferSize(1024);
        SERIAL_DEBUG.setTxBufferSize(1024);
        SERIAL_DEBUG.begin(static_cast<unsigned long>(baud_speed));
        delay(2000);
        println();
        println("-- Starting Helper Initialisation --");

        println("-- Starting Printer Initialisation --");
        Printer::Initialisation(printEnable, printLvl);
        println("-- End of Printer Initialisation --");

        Wifi_Helper::Initialisation();

        println("Creating Incoming Command Queue");
        awaitingCommand = QueueThread<Command>(20);
        if (!awaitingCommand.IsInit())
        {
            println("Error creating the queue : awaitingCommand");
        }

        readBuffer.reserve(readBufferMax);
        readBuffer.clear();

        println("Creating Incoming Command Update Task");
        /* Task function. */
        /* name of task. */
        /* Stack size of task */
        /* parameter of the task */
        /* priority of the task */
        /* Task handle to keep track of created task */
        /* pin task to core 0 */
        taskUpdate = TaskThread(ESP32_Helper::Update, "CommandUpdate");

        println();
        println("+---------------+");
        println("¦  M E C A P I  ¦");
        println("+---------------+");
        println();
        print(__DATE__);
        print(" at ");
        println(__TIME__);
        println();

        println("-- Starting Debugger Initialisation --");
        Debugger::Initialisation();
        Debugger::EnableDebugger(debugEnable);
        println("-- End of Debugger Initialisation --");

        println("-- End of Helper Initialisation --");
        println();
    }

    void Update(void *pvParameters)
    {
        for (;;)
        {
            while (SERIAL_DEBUG.available() > 0)
            {
                ProcessIncomingChar(SERIAL_DEBUG.read());
            }
            vTaskDelay(1);
        }
        Printer::println("Command Update Task STOPPED !");
    }

    void ProcessIncomingChar(char c)
    {
        if (c == '\r')
            return; // Ignore Carriage Return
        if (readBuffer.size() < readBuffer.capacity())
        {
            readBuffer.push_back(c);
            if (c == '\n')
            {
                if (readBuffer.size() > 1)
                {
                    SERIAL_DEBUG.print("Received ");
                    SERIAL_DEBUG.print(readBuffer.size());
                    SERIAL_DEBUG.print(" : ");
                    SERIAL_DEBUG.write(readBuffer.data(), readBuffer.size() - 1);
                    SERIAL_DEBUG.println();
                    ESP32_Helper::BufferReadCommand(readBuffer);
                }
                readBuffer.clear();
            }
        }
        else
        {
            SERIAL_DEBUG.print("SERIAL_DEBUG Read Buffer Overflow : ");
            SERIAL_DEBUG.println(readBuffer.size());
            readBuffer.clear();
        }
    }

    void HandleCommand(Command cmdTmp)
    {
        if(cmdTmp.cmd != "")
        {
            if (cmdTmp.cmd.startsWith("Help"))
            {
                Printer::println("Help Commands : ");
                Printer::println(" > Help");
                Printer::println("     Display this help");
                Printer::println(" > Reboot");
                Printer::println("     Reboot the ESP32");
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
            else if (cmdTmp.cmd == "ChronoPrint" && cmdTmp.size == 1)
            {
                if (cmdTmp.data[0] == 1)
                {
                    // Chrono chrono("", 0);
                    // chrono.EnablePrint(true);
                    println("Enable Chrono Print");
                }
                else
                {
                    // Chrono chrono("", 0);
                    // chrono.EnablePrint(false);
                    println("Disable Chrono Print");
                }
            }
            else
            {
                // If command is not for Lib, we send it to the main
                awaitingCommand.Send(cmdTmp);
            }
        }
    }

    // Read and extract Commands
    void BufferReadCommand(std::vector<char> read)
    {
        Command cmdTmp;
        uint16_t indexSeparator = 0;
        bool isString = false;
        // Start at 1 to let one letter for command at least
        for (int32_t i = 1; i < read.size(); i++)
        {
            if(read[i] != ';' && read[i] != ':' && read[i] != '\n')
            {
                if ((read[i] < 0x30 || read[i] > 0x39) && read[i] != '-' && read[i] != '+')
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
                            else
                                println("String too long !", String(" : "), strToConvert, Level::LEVEL_ERROR);
                        }
                        else
                        {
                            std::size_t pos{};
                            try
                            {
                                int32_t conversion = std::stoi(strToConvert.c_str(), &pos);
                                if (pos == i - indexSeparator)
                                {
                                    cmdTmp.data[cmdTmp.size] = conversion;
                                    cmdTmp.size++;
                                }
                                else
                                {
                                    println("Conversion didn't consume entire string", String(" : "), strToConvert, Level::LEVEL_ERROR);
                                }
                            }
                            catch(const std::exception& e)
                            {
                                println(e.what(), String(" error : "), strToConvert, Level::LEVEL_ERROR);
                            }
                        }
                    }
                    indexSeparator = i + 1;
                }
                isString = false;
            }
        }
        if (cmdTmp.cmd == "Ping")
        {
            // Handle Ping command
            println("Pong");
        }
        else
        {
            Printer::println("Command Received : ", cmdTmp);
            HandleCommand(cmdTmp);
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
    
    // converts character array
    // to string and returns it
    String convertToString(char* a, int32_t size)
    {
        String s = "";
        for (int32_t i = 0; i < size; i++)
        {
            if(a[i] == '\0')
                break;
            s = s + a[i];
        }
        return s;
    }

}
