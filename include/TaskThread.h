#ifndef TASK_THREAD_H
#define TASK_THREAD_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ESP32_Helper.h"

class TaskThread
{
private:
    TaskHandle_t _task;
    TaskFunction_t _pvTaskCode;
    String _pcName;

public:
    bool debugPrint = false;
    TaskThread() = default;
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
            SERIAL_DEBUG.println("Deleted task");
        }
    }

    static void DeleteTask(TaskHandle_t task) { vTaskDelete(task); }
};
#endif
