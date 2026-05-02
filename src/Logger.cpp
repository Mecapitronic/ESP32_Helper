#include "Logger.h"

#include "ESP32_Helper.h"
#include <cstring>

namespace Logger
{
    struct LogMessage
    {
        char text[LOGGER_MESSAGE_SIZE] = {0};
        bool appendNewLine = false;
    };

    namespace
    {
        QueueThread<LogMessage> logQueue;
        bool initialized = false;
        uint32_t droppedMessages = 0;
        portMUX_TYPE statsMutex = portMUX_INITIALIZER_UNLOCKED;

        struct LoggerStats
        {
            uint32_t totalEnqueued = 0;
            uint32_t totalFlushed = 0;
            uint32_t totalDropped = 0;
            uint32_t totalFlushWithData = 0;

            uint32_t windowEnqueued = 0;
            uint32_t windowFlushed = 0;
            uint32_t windowDropped = 0;
            uint32_t windowFlushWithData = 0;
            uint32_t windowMaxMessagesPerFlush = 0;
            uint32_t windowMaxFlushTimeUs = 0;
            uint64_t windowFlushTimeUsAccum = 0;

            uint32_t maxQueueDepth = 0;
            uint32_t lastStatsPrintMs = 0;
            bool statsEnabled = false;
        };

        LoggerStats stats;

        bool HasSerialSpace()
        {
            if (!SERIAL_DEBUG)
                return true;
            return SERIAL_DEBUG.availableForWrite() > 0;
        }
    }

    void Initialisation()
    {
        if (initialized)
            return;

        logQueue = QueueThread<LogMessage>(LOGGER_QUEUE_SIZE);
        initialized = logQueue.IsInit();
        droppedMessages = 0;
        stats = LoggerStats{};
        stats.lastStatsPrintMs = millis();

        if (!initialized)
        {
            if (SERIAL_DEBUG)
                SERIAL_DEBUG.println("Logger init failed");
        }
    }

    bool IsInitialized()
    {
        return initialized;
    }

    bool Enqueue(const char *text, bool appendNewLine)
    {
        if (!initialized)
        {
            if (text == nullptr)
                text = "";
            if (SERIAL_DEBUG)
            {
                if (appendNewLine)
                    SERIAL_DEBUG.println(text);
                else
                    SERIAL_DEBUG.print(text);
            }

            if (Wifi_Helper::IsEnable() && Wifi_Helper::IsClientConnected())
            {
                if (appendNewLine)
                    WIFI_DEBUG.println(text);
                else
                    WIFI_DEBUG.print(text);
            }
            return false;
        }

        LogMessage msg;
        msg.appendNewLine = appendNewLine;

        if (text == nullptr)
            text = "";

        snprintf(msg.text, LOGGER_MESSAGE_SIZE, "%s", text);

        if (logQueue.MessagesWaiting() >= LOGGER_QUEUE_SIZE)
        {
            droppedMessages++;
            portENTER_CRITICAL(&statsMutex);
            stats.totalDropped++;
            stats.windowDropped++;
            portEXIT_CRITICAL(&statsMutex);
            return false;
        }

        logQueue.Send(msg);

        portENTER_CRITICAL(&statsMutex);
        stats.totalEnqueued++;
        stats.windowEnqueued++;
        const uint32_t queueDepth = static_cast<uint32_t>(logQueue.MessagesWaiting());
        if (queueDepth > stats.maxQueueDepth)
            stats.maxQueueDepth = queueDepth;
        portEXIT_CRITICAL(&statsMutex);

        return true;
    }

    void Flush(uint16_t maxMessages)
    {
        if (!initialized)
            return;

        if (maxMessages == 0)
            maxMessages = 1;

        const uint32_t flushStartUs = micros();

        if (droppedMessages > 0)
        {
            if (HasSerialSpace())
            {
                if (SERIAL_DEBUG)
                {
                    SERIAL_DEBUG.print("Logger dropped messages: ");
                    SERIAL_DEBUG.println(droppedMessages);
                }
                droppedMessages = 0;
            }
        }

        LogMessage msg;
        uint16_t count = 0;
        while (count < maxMessages)
        {
            if ((micros() - flushStartUs) >= LOGGER_FLUSH_TIME_BUDGET_US)
                break;

            // Do not pop from queue if UART TX buffer is already full.
            if (!HasSerialSpace())
                break;

            if (!logQueue.Receive(msg))
                break;

            if (msg.appendNewLine)
            {
                if (SERIAL_DEBUG)
                    SERIAL_DEBUG.println(msg.text);
            }
            else
            {
                if (SERIAL_DEBUG)
                    SERIAL_DEBUG.print(msg.text);
            }

            if (Wifi_Helper::IsEnable() && Wifi_Helper::IsClientConnected())
            {
                if (msg.appendNewLine)
                    WIFI_DEBUG.println(msg.text);
                else
                    WIFI_DEBUG.print(msg.text);
            }

            count++;
        }

        const uint32_t flushDurationUs = micros() - flushStartUs;
        if (count > 0)
        {
            portENTER_CRITICAL(&statsMutex);
            stats.totalFlushed += count;
            stats.windowFlushed += count;
            stats.totalFlushWithData++;
            stats.windowFlushWithData++;
            if (count > stats.windowMaxMessagesPerFlush)
                stats.windowMaxMessagesPerFlush = count;
            if (flushDurationUs > stats.windowMaxFlushTimeUs)
                stats.windowMaxFlushTimeUs = flushDurationUs;
            stats.windowFlushTimeUsAccum += flushDurationUs;
            portEXIT_CRITICAL(&statsMutex);
        }

        PrintStats(false);
    }

    void PrintStats(bool force)
    {
        if (!initialized)
            return;

        const uint32_t nowMs = millis();

        uint32_t elapsedMs = 0;
        uint32_t totalEnqueuedSnapshot = 0;
        uint32_t totalFlushedSnapshot = 0;
        uint32_t totalDroppedSnapshot = 0;
        uint32_t maxQueueDepthSnapshot = 0;

        uint32_t windowEnqueuedSnapshot = 0;
        uint32_t windowFlushedSnapshot = 0;
        uint32_t windowDroppedSnapshot = 0;
        uint32_t windowFlushWithDataSnapshot = 0;
        uint32_t windowMaxMessagesPerFlushSnapshot = 0;
        uint32_t windowMaxFlushTimeUsSnapshot = 0;
        uint64_t windowFlushTimeUsAccumSnapshot = 0;

        portENTER_CRITICAL(&statsMutex);
        elapsedMs = nowMs - stats.lastStatsPrintMs;
        if (!force && (!stats.statsEnabled || elapsedMs < LOGGER_STATS_PERIOD_MS))
        {
            portEXIT_CRITICAL(&statsMutex);
            return;
        }

        if (elapsedMs == 0)
            elapsedMs = 1;

        totalEnqueuedSnapshot = stats.totalEnqueued;
        totalFlushedSnapshot = stats.totalFlushed;
        totalDroppedSnapshot = stats.totalDropped;
        maxQueueDepthSnapshot = stats.maxQueueDepth;

        windowEnqueuedSnapshot = stats.windowEnqueued;
        windowFlushedSnapshot = stats.windowFlushed;
        windowDroppedSnapshot = stats.windowDropped;
        windowFlushWithDataSnapshot = stats.windowFlushWithData;
        windowMaxMessagesPerFlushSnapshot = stats.windowMaxMessagesPerFlush;
        windowMaxFlushTimeUsSnapshot = stats.windowMaxFlushTimeUs;
        windowFlushTimeUsAccumSnapshot = stats.windowFlushTimeUsAccum;

        stats.windowEnqueued = 0;
        stats.windowFlushed = 0;
        stats.windowDropped = 0;
        stats.windowFlushWithData = 0;
        stats.windowMaxMessagesPerFlush = 0;
        stats.windowMaxFlushTimeUs = 0;
        stats.windowFlushTimeUsAccum = 0;
        stats.lastStatsPrintMs = nowMs;
        portEXIT_CRITICAL(&statsMutex);

        const float windowSec = static_cast<float>(elapsedMs) / 1000.0f;
        const float messagesPerSec = static_cast<float>(windowFlushedSnapshot) / windowSec;
        const float avgMessagesPerFlush = windowFlushWithDataSnapshot > 0
                                              ? static_cast<float>(windowFlushedSnapshot) / static_cast<float>(windowFlushWithDataSnapshot)
                                              : 0.0f;
        const float avgFlushTimeUs = windowFlushWithDataSnapshot > 0
                                         ? static_cast<float>(windowFlushTimeUsAccumSnapshot) / static_cast<float>(windowFlushWithDataSnapshot)
                                         : 0.0f;

        char line[256];
        snprintf(line,
                 sizeof(line),
                 "[Logger %lums] msg/s=%.1f enq=%lu out=%lu drop=%lu avgMsg/flush=%.2f maxMsg/flush=%lu avgFlushUs=%.1f maxFlushUs=%lu qPeak=%lu",
                 static_cast<unsigned long>(elapsedMs),
                 messagesPerSec,
                 static_cast<unsigned long>(windowEnqueuedSnapshot),
                 static_cast<unsigned long>(windowFlushedSnapshot),
                 static_cast<unsigned long>(windowDroppedSnapshot),
                 avgMessagesPerFlush,
                 static_cast<unsigned long>(windowMaxMessagesPerFlushSnapshot),
                 avgFlushTimeUs,
                 static_cast<unsigned long>(windowMaxFlushTimeUsSnapshot),
                 static_cast<unsigned long>(maxQueueDepthSnapshot));
        if (SERIAL_DEBUG)
            SERIAL_DEBUG.println(line);
        if (Wifi_Helper::IsEnable() && Wifi_Helper::IsClientConnected())
            WIFI_DEBUG.println(line);

        snprintf(line,
                 sizeof(line),
                 "[Logger total] enq=%lu out=%lu drop=%lu waiting=%ld",
                 static_cast<unsigned long>(totalEnqueuedSnapshot),
                 static_cast<unsigned long>(totalFlushedSnapshot),
                 static_cast<unsigned long>(totalDroppedSnapshot),
                 static_cast<long>(logQueue.MessagesWaiting()));
        if (SERIAL_DEBUG)
            SERIAL_DEBUG.println(line);
        if (Wifi_Helper::IsEnable() && Wifi_Helper::IsClientConnected())
            WIFI_DEBUG.println(line);
    }

    uint32_t DroppedMessages()
    {
        return droppedMessages;
    }

    bool HandleCommand(Command cmdTmp)
    {
        if (cmdTmp.cmdEquals("LoggerStats"))
        {
            // LoggerStats:1  -> activer impression auto toutes les LOGGER_STATS_PERIOD_MS
            // LoggerStats:0  -> désactiver
            // LoggerStats    -> afficher maintenant
            if (cmdTmp.size == 0)
            {
                PrintStats(true);
            }
            else if (cmdTmp.size >= 1)
            {
                portENTER_CRITICAL(&statsMutex);
                stats.statsEnabled = (cmdTmp.data[0] != 0);
                portEXIT_CRITICAL(&statsMutex);
                if (SERIAL_DEBUG)
                {
                    SERIAL_DEBUG.print("Logger stats auto print: ");
                    SERIAL_DEBUG.println(cmdTmp.data[0] != 0 ? "enabled" : "disabled");
                }
                if (Wifi_Helper::IsEnable() && Wifi_Helper::IsClientConnected())
                {
                    WIFI_DEBUG.print("Logger stats auto print: ");
                    WIFI_DEBUG.println(cmdTmp.data[0] != 0 ? "enabled" : "disabled");
                }
            }
        }
        else if (cmdTmp.cmdEquals("LoggerResetStats"))
        {
            // LoggerResetStats
            portENTER_CRITICAL(&statsMutex);
            stats.totalEnqueued = 0;
            stats.totalFlushed = 0;
            stats.totalDropped = 0;
            stats.totalFlushWithData = 0;
            stats.windowEnqueued = 0;
            stats.windowFlushed = 0;
            stats.windowDropped = 0;
            stats.windowFlushWithData = 0;
            stats.windowMaxMessagesPerFlush = 0;
            stats.windowMaxFlushTimeUs = 0;
            stats.windowFlushTimeUsAccum = 0;
            stats.maxQueueDepth = 0;
            stats.lastStatsPrintMs = millis();
            portEXIT_CRITICAL(&statsMutex);
            if (SERIAL_DEBUG)
                SERIAL_DEBUG.println("Logger stats reset.");
            if (Wifi_Helper::IsEnable() && Wifi_Helper::IsClientConnected())
                WIFI_DEBUG.println("Logger stats reset.");
        }
        else
        {
            Printer::println("Not a Logger command !");
            return false;
        }
        return true;
    }

    void PrintCommandHelp()
    {
        Printer::println("Logger Command Help :");
        Printer::println(" > LoggerStats");
        Printer::println("      Affiche les stats maintenant");
        Printer::println(" > LoggerStats:[int]");
        Printer::println("      0 Desactiver, 1 Activer l'affichage auto toutes les %dms", LOGGER_STATS_PERIOD_MS);
        Printer::println(" > LoggerResetStats");
        Printer::println("      Remet tous les compteurs a zero");
        Printer::println();
    }
}
