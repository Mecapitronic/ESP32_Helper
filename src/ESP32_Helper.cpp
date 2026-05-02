#include "ESP32_Helper.h"

using namespace Printer;
using namespace std;

namespace ESP32_Helper
{
    namespace // anonymous nested namespace, cannot access outside this file
    {
        struct CommandHandler
        {
            String prefix;
            CommandHandlerFunc func;
            CommandHelpFunc helpFunc;
        };

        const int8_t readBufferMax = 64;
        std::vector<char> readBuffer;
        std::vector<CommandHandler> customHandlers;
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
        delay(2000);

        Logger::Initialisation();
        loggerFlushTimer = TimerThread(LoggerFlushCallback, "LoggerFlush", pdMS_TO_TICKS(LOGGER_FLUSH_PERIOD_MS));
        loggerFlushTimer.Start();

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

        println("-- Starting SPIFFS Initialisation --");
        if (!FileSystem_Helper::Initialisation())
        {
            println("Error initialising the SPIFFS");
        }
        FileSystem_Helper::ListFiles();
        println("-- End of SPIFFS Initialisation --");

        // Register default namespace handlers through the same extension mechanism.
        customHandlers.clear();
        RegisterCommandHandler("Debug", Debugger::HandleCommand, Debugger::PrintCommandHelp);
        RegisterCommandHandler("Print", Printer::HandleCommand, Printer::PrintCommandHelp);
        RegisterCommandHandler("Pref", Preferences_Helper::HandleCommand, Preferences_Helper::PrintCommandHelp);
        RegisterCommandHandler("Wifi", Wifi_Helper::HandleCommand, Wifi_Helper::PrintCommandHelp);
        RegisterCommandHandler("Logger", Logger::HandleCommand, Logger::PrintCommandHelp);
        RegisterCommandHandler("SPIFFS", FileSystem_Helper::HandleCommand, FileSystem_Helper::PrintCommandHelp);

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
                    Printer::print("Received %u : ", static_cast<unsigned int>(readBuffer.size()));
                    Printer::println(String(readBuffer.data(), readBuffer.size() - 1));
                    ESP32_Helper::BufferReadCommand(readBuffer);
                }
                readBuffer.clear();
            }
        }
        else
        {
            Printer::println("SERIAL_DEBUG Read Buffer Overflow : %u", static_cast<unsigned int>(readBuffer.size()));
            readBuffer.clear();
        }
    }

    void RegisterCommandHandler(const String &prefix, CommandHandlerFunc handler, CommandHelpFunc helpFunc)
    {
        if (prefix == "" || handler == nullptr)
        {
            Printer::println("Invalid Command Handler registration for prefix: " + prefix);
            return;
        }
        customHandlers.push_back({prefix, handler, helpFunc});
    }

    bool HandleCommand(Command cmdTmp)
    {
        if (cmdTmp.cmd[0] == '\0')
            return false;

        if (cmdTmp.cmdStartsWith("Help"))
        {
            Printer::println("Help Commands : ");
            Printer::println(" > Help");
            Printer::println("     Display this help");
            Printer::println(" > Reboot");
            Printer::println("     Reboot the ESP32");
            for (const auto &handler : customHandlers)
            {
                if (handler.helpFunc != nullptr)
                    handler.helpFunc();
            }
            return true;
        }
        else if (cmdTmp.cmdStartsWith("Reboot"))
        {
            Printer::println("Rebooting...");
            ESP.restart();
            return true;
        }
        else if (cmdTmp.cmdEquals("Ping"))
        {
            // Handle Ping command
            println("Pong");
            return true;
        }
        else
        {
            // Check all registered handlers (default namespaces + user custom handlers)
            bool handled = false;
            for (const auto &handler : customHandlers)
            {
                if (cmdTmp.cmdStartsWith(handler.prefix.c_str()))
                {
                    handled = handler.func(cmdTmp);
                    if (!handled && handler.helpFunc != nullptr)
                        handler.helpFunc();
                    return handled;
                }
            }

            // If no custom handler found, send to main application queue
            // Queue by value: Command is now pure POD (char arrays), safe for FreeRTOS
            awaitingCommand.Send(cmdTmp);
            return false;
        }
    }

    // Read and extract Commands
    void BufferReadCommand(std::vector<char> read)
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
                if (cmdTmp.cmd[0] == '\0')
                {
                    if (i >= Command::sizeCmd)
                    {
                        println("Command name too long: max %i chars", Command::sizeCmd - 1);
                        readBuffer.clear();
                        return;
                    }
                    snprintf(cmdTmp.cmd, Command::sizeCmd, "%.*s", (int)i, &read[0]);
                    indexSeparator = i + 1;
                }
                else
                {
                    if (i - indexSeparator > 0)
                    {
                        // check if it's a string, if it's an integer : convert it
                        String strToConvert = String(&read[indexSeparator], i - indexSeparator);
                        if (isString)
                        {
                            if (i - indexSeparator < Command::sizeStr)
                            {
                                if (cmdTmp.dataStr1[0] == '\0')
                                    snprintf(cmdTmp.dataStr1, Command::sizeStr, "%s", strToConvert.c_str());
                                else if (cmdTmp.dataStr2[0] == '\0')
                                    snprintf(cmdTmp.dataStr2, Command::sizeStr, "%s", strToConvert.c_str());
                                else
                                    println("3rd String not saved: " + strToConvert);
                            }
                            else
                                println("String too long: " + strToConvert);
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
                                    println("Conversion didn't consume entire string: " + strToConvert);
                                }
                            }
                            catch (const std::exception &e)
                            {
                                println("Exception error: " + String(e.what()) + " for data: " + strToConvert);
                            }
                        }
                        else
                        {
                            println("Data array full, data not saved: " + strToConvert);
                        }
                    }
                    indexSeparator = i + 1;
                }
                isString = false;
            }
        }
        Printer::println("Command Received : %s", cmdTmp.ToString().c_str());
        HandleCommand(cmdTmp);
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
