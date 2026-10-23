/**
 * @file Logger.h
 * @brief Thread-safe logger queue for serial and wifi outputs
 */
#ifndef LOGGER_H
#define LOGGER_H

#include "Structure_Helper.h"

#ifndef LOGGER_QUEUE_SIZE
#define LOGGER_QUEUE_SIZE 128
#endif

#ifndef LOGGER_MESSAGE_SIZE
#define LOGGER_MESSAGE_SIZE 256
#endif

#ifndef LOGGER_FLUSH_PERIOD_MS
#define LOGGER_FLUSH_PERIOD_MS 10
#endif

#ifndef LOGGER_MAX_FLUSH_BATCH
#define LOGGER_MAX_FLUSH_BATCH 64
#endif

#ifndef LOGGER_FLUSH_TIME_BUDGET_US
#define LOGGER_FLUSH_TIME_BUDGET_US 1000
#endif

#ifndef LOGGER_STATS_PERIOD_MS
#define LOGGER_STATS_PERIOD_MS 5000
#endif

namespace Logger
{
    void Initialisation();
    void RegisterCommands();
    bool IsInitialized();

    bool Enqueue(const char *text, bool appendNewLine = false);
    void Flush(uint16_t maxMessages = LOGGER_MAX_FLUSH_BATCH);
    void PrintStats(bool force = false);

    uint32_t DroppedMessages();
    void EnableStats(bool enable);
    void ResetStats();
}

#endif
