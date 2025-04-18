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
    bool debug = false;

    void task()
    {
        if (debug)
        {
            SERIAL_DEBUG.print("Calling Task : ");
            SERIAL_DEBUG.println(_pcName);
        }
        _pvTaskCode(NULL);
    }

    static void startTaskImpl(void *_this)
    {
        if (((TaskThread *)_this)->debug)
        {
            SERIAL_DEBUG.print("Impl Task : ");
            SERIAL_DEBUG.println(((TaskThread *)_this)->_pcName);
        }
        ((TaskThread *)_this)->task();
    }

    static void *startThread(void *_this)
    {
        ((TaskThread *)_this)->task();
        return NULL;
    }

public:
    TaskThread() {}

    TaskThread(TaskFunction_t pvTaskCode, const char *const pcName, const uint32_t usStackDepth = 10000,
               UBaseType_t uxPriority = 5, const BaseType_t xCoreID = 0) : _pvTaskCode(pvTaskCode), _pcName(pcName)
    {
        if (debug)
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
        xTaskCreatePinnedToCore(TaskThread::startTaskImpl, pcName, usStackDepth, this, uxPriority, &_task, xCoreID);
    }

    ~TaskThread()
    {
        if (debug)
        {
            SERIAL_DEBUG.print("Deleting Task : ");
            SERIAL_DEBUG.println(_pcName);
            SERIAL_DEBUG.println("Deleted task");
        }
    }

    // get stack for task 2
    // unsigned int temp2 = uxTaskGetStackHighWaterMark(nullptr);
    // SERIAL_DEBUG.print("task2="); SERIAL_DEBUG.println(temp2);
    // log_i( "fDoTheHumidityThing high watermark %d",  uxTaskGetStackHighWaterMark( NULL ) );
};
#endif
