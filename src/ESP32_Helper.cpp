#include "ESP32_Helper.h"

using namespace Printer;
using namespace std;

namespace ESP32_Helper
{
    namespace // anonymous nested namespace, cannot access outside this file
    {
        const int8_t readBufferMax = 64;
        std::vector<char> readBuffer;

        std::vector<CommandHandler> commandList;
        TaskThread taskUpdate;
        QueueThread<Command> awaitingCommand;
        TimerThread loggerFlushTimer;

        void LoggerFlushCallback(TimerHandle_t xTimer)
        {
            //(void)xTimer;
            Logger::Flush();
        }
    }

    void Initialisation(BaudRate baud_speed, Enable printEnable, Level printLvl, Enable debugEnable)
    {
        SERIAL_DEBUG.end();
        SERIAL_DEBUG.setRxBufferSize(1024);
        SERIAL_DEBUG.setTxBufferSize(1024);
        SERIAL_DEBUG.begin(static_cast<unsigned long>(baud_speed));
        /*Timeout serialStartTimeout;
        serialStartTimeout.Start(2000);
        while (!SERIAL_DEBUG && !serialStartTimeout.IsTimeOut())
        {
            vTaskDelay(10);
        }*/
        commandList.clear();
        RegisterCommand("Delay", "Delay:<duration:int@ms> - Delay execution; duration > 0. Example: Delay:1000", FunctionCommand( if(cmd.hasData() && cmd.data[0] > 0) { Printer::println("Delaying for %d ms...", cmd.data[0]); vTaskDelay(cmd.data[0]); Printer::println("Delay complete."); } else { Printer::println("Delay command requires a duration in ms as data."); } ));
        RegisterCommand("Reboot", "Reboot - Reboot the system.", FunctionCommand( Printer::println("Rebooting..."); ESP.restart(); ));
        RegisterCommand("Ping", "Ping - Reply with Pong.", FunctionCommand( Printer::println("Pong"); ));

        //Logger::Initialisation();
        //loggerFlushTimer = TimerThread(LoggerFlushCallback, "LoggerFlush", pdMS_TO_TICKS(LOGGER_FLUSH_PERIOD_MS));
        //loggerFlushTimer.Start();

        println();
        println("+---------------+");
        println("¦  M E C A P I  ¦");
        println("+---------------+");
        println();
        println("%s at %s", __DATE__, __TIME__);
        println();

        println("-- Starting ESP32 Helper Initialisation --");
        
        println("-- Starting Preferences Initialisation --");
        Preferences_Helper::Initialisation();
        println("-- End of Preferences Initialisation --");

        println("-- Starting Printer Initialisation --");
        Printer::Initialisation(printEnable, printLvl);
        println("-- End of Printer Initialisation --");

        println("-- Starting WiFi Initialisation --");
        Wifi_Helper::Initialisation();
        println("-- End of WiFi Initialisation --");

        println("Creating Incoming Command Queue");
        awaitingCommand = QueueThread<Command>(20);
        if (!awaitingCommand.IsInit())
        {
            println("Error creating the queue awaitingCommand");
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

        println("-- Starting Debugger Initialisation --");
        Debugger::Initialisation();
        Debugger::EnableDebugger(debugEnable);
        println("-- End of Debugger Initialisation --");

        println("-- Starting SPIFFS Initialisation --");
        if (!FileSystem_Helper::Initialisation())
        {
            println("Error initialising the SPIFFS");
        }
        FileSystem_Helper::ListFiles();
        println("-- End of SPIFFS Initialisation --");

        println("-- End of ESP32 Helper Initialisation --");
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
            if(FileSystem_Helper::WriteASCII::timeout.IsTimeOut())
            {
                Printer::println("Serial ASCII session timed out for file: %s with %u/%u bytes", FileSystem_Helper::WriteASCII::fileName.c_str(), static_cast<unsigned int>(FileSystem_Helper::WriteASCII::buffer.size()), static_cast<unsigned int>(FileSystem_Helper::WriteASCII::bufferSize));
                FileSystem_Helper::WriteASCII::Reset();
            }
            vTaskDelay(1);
        }
        Printer::println("Command Update Task STOPPED !");
    }

    void ProcessIncomingChar(char c)
    {
       if (FileSystem_Helper::WriteASCII::timeout.IsRunning())
       {
           FileSystem_Helper::WriteASCII::buffer.push_back(static_cast<uint8_t>(c));
           // Timeout will be restarted with each incoming character
           FileSystem_Helper::WriteASCII::timeout.Start();
           if (FileSystem_Helper::WriteASCII::buffer.size() >= FileSystem_Helper::WriteASCII::bufferSize)
           {
               FileSystem_Helper::WriteASCII::timeout.Stop();
               Printer::println("Processing complete ASCII session for file: %s with %u bytes", FileSystem_Helper::WriteASCII::fileName.c_str(), static_cast<unsigned int>(FileSystem_Helper::WriteASCII::buffer.size()));               
               if(FileSystem_Helper::WriteASCII::overwrite)
               {
                    if (FileSystem_Helper::WriteFile(FileSystem_Helper::WriteASCII::fileName, FileSystem_Helper::WriteASCII::buffer, true))
                    {
                        Printer::println("Successfully overwrote file: %s", FileSystem_Helper::WriteASCII::fileName.c_str());
                    }
                    else
                    {
                        Printer::println("Failed to overwrite file: %s", FileSystem_Helper::WriteASCII::fileName.c_str());
                    }
               }
               else
               {
                    if (FileSystem_Helper::AppendFile(FileSystem_Helper::WriteASCII::fileName, FileSystem_Helper::WriteASCII::buffer, true))
                    {
                        Printer::println("Successfully appended to file: %s", FileSystem_Helper::WriteASCII::fileName.c_str());
                    }
                    else
                    {
                        Printer::println("Failed to append to file: %s", FileSystem_Helper::WriteASCII::fileName.c_str());
                    }
                }
               FileSystem_Helper::WriteASCII::Reset();
           }
           return;
       }
        if (c == '\r')
            return; // Ignore Carriage Return
        if (readBuffer.size() < readBuffer.capacity())
        {
            readBuffer.push_back(c);
            if (c == '\n' || c == ';')
            {
                if (readBuffer.size() > 1)
                {
                    String receivedCmd(readBuffer.data(), readBuffer.size() - 1);
                    Printer::println("Received %u %s", static_cast<unsigned int>(readBuffer.size()), receivedCmd.c_str());
                    Command command = ESP32_Helper::ParseCommandBuffer(readBuffer);
                    ESP32_Helper::HandleCommand(command);
                }
                readBuffer.clear();
            }
        }
        else
        {
            Printer::println("SERIAL_DEBUG Read Buffer Overflow %u", static_cast<unsigned int>(readBuffer.size()));
            readBuffer.clear();
        }
    }

    void RegisterCommand(const String &command, const String &help, CommandHandlerFunc func)
    {
        if (command == "" || func == nullptr)
        {
            Printer::println("Invalid Command registration for command " + command);
            return;
        }
        commandList.push_back(CommandHandler(command, help, func));
        /*        
        ESP32_Helper::RegisterCommand("", "",
            FunctionCommand(
                if (cmd.hasData()) {

                } ));
        */
    }

    bool HandleCommand(Command cmdTmp)
    {
        if (!cmdTmp.hasCommand())
            return false;

        if (cmdTmp.cmdEquals("Help"))
        {
            Printer::println("Help Commands List Start");
            Printer::println("Syntax: Command:<name:type@unit>(:name:type@unit=default) - Description");
            Printer::println("<...> required; (...) optional, including its ':'; =default when omitted.");
            Printer::println("Types: int (signed 32-bit), string, bool (0=false, 1=true), int|string (either).");
            Printer::println("float denotes a decimal, only if explicitly handled by the command; not parsed natively.");
            Printer::println("%s", "Units after '@': ms, s, %, deg, mm, mm/s, bit/s, byte; omitted when not applicable.");
            Printer::println("Do not type annotations, brackets or parentheses. Example: Delay:1000");
            Printer::println("Separate arguments with ':'; end commands with newline or ';'. No quoting/escaping.");
            Printer::println("Numeric-only arguments are integers, not strings; ':' and ';' cannot occur in strings.");
            Printer::println("Limits: 24 characters per name/string, 6 integers, 2 strings; 63 characters per command.");
            Printer::println("Multiple signatures describe distinct usages; constraints are documented, not auto-validated.");
            Printer::println(" > Help - List commands and explain this notation.");
            for (const auto &handler : commandList)
            {
                Printer::println(" > " + handler.command);
                Printer::println("     " + handler.help);
            }
            Printer::println("Help Commands List End");
            return true;
        }
        else
        {
            // Check all registered handlers (default namespaces + user custom handlers)
            for (const auto &cmd : commandList)
            {
                if (cmdTmp.cmdEquals(cmd.command.c_str()))
                {
                    cmd.func(cmdTmp);
                    return true;
                }
            }

            // If no command found, send to main application queue
            // Queue by value: Command is now pure POD (char arrays), safe for FreeRTOS
            Printer::println("No command found for " + cmdTmp.ToString());
            awaitingCommand.Send(cmdTmp);
            return false;
        }
    }

    // Read and extract Commands
    Command ParseCommandBuffer(const std::vector<char> &read)
    {
        Command cmdTmp;
        uint16_t indexSeparator = 0;
        bool isString = false;
        // Start at 1 to let one letter for command at least
        for (size_t i = 1; i < read.size(); i++)
        {
            if (read[i] != ';' && read[i] != ':' && read[i] != '\n')
            {
                if ((read[i] < 0x30 || read[i] > 0x39) && read[i] != '-' && read[i] != '+')
                    isString = true;
            }
            else
            {
                if (!cmdTmp.hasCommand())
                {
                    if (i >= Command::sizeCmd)
                    {
                        println("Command name too long max %i chars", Command::sizeCmd - 1);
                        readBuffer.clear();
                        return cmdTmp;
                    }
                    snprintf(cmdTmp.cmd, Command::sizeCmd, "%.*s", (int)i, &read[0]);
                    indexSeparator = i + 1;
                }
                else
                {
                    if (i - indexSeparator > 0)
                    {
                        // check if it's a string, if it's an integer convert it
                        String strToConvert = String(&read[indexSeparator], i - indexSeparator);
                        if (isString)
                        {
                            if (i - indexSeparator < Command::sizeStr)
                            {
                                if (!cmdTmp.hasDataStr1())
                                    snprintf(cmdTmp.dataStr1, Command::sizeStr, "%s", strToConvert.c_str());
                                else if (!cmdTmp.hasDataStr2())
                                    snprintf(cmdTmp.dataStr2, Command::sizeStr, "%s", strToConvert.c_str());
                                else
                                    println("3rd String not saved " + strToConvert);
                            }
                            else
                                println("String too long " + strToConvert);
                        }
                        else if (cmdTmp.size < Command::length)
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
                                    println("Conversion didn't consume entire string " + strToConvert);
                                }
                            }
                            catch (const std::exception &e)
                            {
                                println("Exception error " + String(e.what()) + " for data " + strToConvert);
                            }
                        }
                        else
                        {
                            println("Data array full, data not saved " + strToConvert);
                        }
                    }
                    indexSeparator = i + 1;
                }
                isString = false;
            }
        }
        Printer::println("Command Received %s", cmdTmp.ToString().c_str());
        return cmdTmp;
    }

    bool HasWaitingCommand() { return awaitingCommand.MessagesWaiting() > 0; }

    Command GetCommand()
    {
        if (!HasWaitingCommand())
        {
            return nullptr; // No command available
        }
        Command cmd;
        awaitingCommand.Receive(cmd);
        return cmd;
    }

    // converts character array
    // to string and returns it
    String convertToString(char *a, int32_t size)
    {
        String s = "";
        for (int32_t i = 0; i < size; i++)
        {
            if (a[i] == '\0')
                break;
            s = s + a[i];
        }
        return s;
    }

}
