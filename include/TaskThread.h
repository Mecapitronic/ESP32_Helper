#ifndef TASK_THREAD_H
#define TASK_THREAD_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ESP32_Helper.h"

class TaskThread
{
private:
    TaskHandle_t _task = nullptr;
    TaskFunction_t _pvTaskCode = nullptr;
    String _pcName = "";

public:
    bool debugPrint = false;
    TaskThread() = default;
    /**
     * @brief Launch a new Task Thread
     *
     * @param pvTaskCode The function to call
     * @param pcName The name of the thread
     * @param usStackDepth Length of stack
     * @param uxPriority Priority, from High (20) to Low (0)
     * @param xCoreID Core affinity
     */
    TaskThread(TaskFunction_t pvTaskCode, const char *const pcName, const uint32_t usStackDepth = 10000,
               UBaseType_t uxPriority = 5, const BaseType_t xCoreID = 0) : _pvTaskCode(pvTaskCode), _pcName(pcName)
    {
        if (debugPrint)
        {
            SERIAL_DEBUG.print("Creating Task : ");
            SERIAL_DEBUG.println(_pcName);
        }

        /* Task function. */
        /* name of task. */
        /* Stack size of task */
        /* parameter of the task */
        /* priority of the task */
        /* Task handle to keep track of created task */
        /* pin task to core x */
        xTaskCreatePinnedToCore(pvTaskCode, pcName, usStackDepth, this, uxPriority, &_task, xCoreID);
    }

    ~TaskThread()
    {
        if (debugPrint)
        {
            SERIAL_DEBUG.print("Deleting Task : ");
            SERIAL_DEBUG.println(_pcName);
        }
    }

    static void DeleteTask(TaskHandle_t task) { vTaskDelete(task); }
};
#endif
