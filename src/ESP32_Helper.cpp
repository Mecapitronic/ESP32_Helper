#include "ESP32_Helper.h"

using namespace Printer;

namespace ESP32_Helper
{
    namespace // anonymous nested namespace, cannot access outside this file
    {
        TaskHandle_t TaskUpdate;
        QueueHandle_t awaitingCommand = NULL;

        // converts character array
        // to string and returns it
        String convertToString(char *a, int size)
        {
            int i;
            String s = "";
            for (i = 0; i < size; i++)
            {
                s = s + a[i];
            }
            return s;
        }
    }

    void Initialisation(int baud_speed, Enable printEnable, Level printLvl, Enable debugEnable)
    {
        Serial.end();
        Serial.setRxBufferSize(1024);
        Serial.setTxBufferSize(1024);
        Serial.begin(baud_speed);
        Serial.println();
        Serial.println("-- Starting Helper Initialisation --");
        Wifi_Helper::Initialisation();
        Serial.println("Creating Incoming Command Queue");
        awaitingCommand = xQueueCreate(100, sizeof(Command));
        if (awaitingCommand == NULL)
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
        xTaskCreatePinnedToCore(ESP32_Helper::Update, "CommandUpdate", 10000, NULL, 5, &TaskUpdate, 0);

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
        uint16_t indexBuffer = 0;
        const int readBufferMax = 64;
        char readBuffer[readBufferMax];

        for (;;)
        {
            while (SERIAL_DEBUG.available() > 0)
            {
                char tmpChar = SERIAL_DEBUG.read();
                if (indexBuffer < readBufferMax)
                {
                    readBuffer[indexBuffer++] = tmpChar;
                    if (tmpChar == '\n')
                    {
                        SERIAL_DEBUG.print("Received ");
                        SERIAL_DEBUG.print(indexBuffer);
                        SERIAL_DEBUG.print(" : ");
                        SERIAL_DEBUG.write(readBuffer, indexBuffer);
                        SERIAL_DEBUG.println();
                        BufferReadCommand(readBuffer, indexBuffer);
                        indexBuffer = 0;
                    }
                }
                else
                {
                    SERIAL_DEBUG.print("Read Buffer Overflow : ");
                    SERIAL_DEBUG.println(indexBuffer);
                    indexBuffer = 0;
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
                        if(i - indexSeparator <= cmdTmp.sizeStr)
                            cmdTmp.dataStr = strToConvert;                        
                        //else
                        //   println("String too long !");
                    }
                }
                catch(const std::exception& e)
                {
                    //println("Conversion error: not an integer");                    
                    if(i - indexSeparator <= cmdTmp.sizeStr)
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
            else if (cmdTmp.cmd.startsWith("Wifi"))
                Wifi_Helper::HandleCommand(cmdTmp);
            else
            {
                // If command is not for Lib, we sent it to the main
                xQueueSend(awaitingCommand, &cmdTmp, 0);
            }
        }
    }

    bool HasWaitingCommand() { return uxQueueMessagesWaiting(awaitingCommand) > 0; }

    Command GetCommand()
    {
        Command cmd;
        if (HasWaitingCommand())
        {
            if (xQueueReceive(awaitingCommand, &cmd, portTICK_PERIOD_MS * 0))
            {
                return cmd;
            }
            else
            {
                cmd.cmd = "";
            }
        }
        return cmd;
    }

    int64_t GetTimeNowMs()
    {
        micros();
        struct timeval tv_now;
        gettimeofday(&tv_now, NULL);

        int64_t time_ms = (int64_t)tv_now.tv_sec * 1000 + ((int64_t)tv_now.tv_usec / 1000);
        return time_ms;
    }

    int64_t GetTimeNowUs()
    {
        struct timeval tv_now;
        gettimeofday(&tv_now, NULL);

        int64_t time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
        return time_us;
    }
}
