#ifndef TASK_THREAD_H
#define TASK_THREAD_H

#ifdef _VISUAL_STUDIO
#include <pthread.h>
#define portBASE_TYPE int
typedef portBASE_TYPE BaseType_t;
typedef unsigned portBASE_TYPE UBaseType_t;

typedef void *TaskHandle_t;
typedef void (*TaskFunction_t)(void *);
#else
#include "ESP32_Helper.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif
static const bool debugPrint = false;

class TaskThread
{
private:
#ifdef _VISUAL_STUDIO
    pthread_t pthread;
    TaskFunction_t _pvTaskCode = nullptr;
    const char *_pcName;
#else
    TaskHandle_t _task = nullptr;
    TaskFunction_t _pvTaskCode = nullptr;
    String _pcName = "";
#endif

#ifdef _VISUAL_STUDIO
    void task()
    {
        if (debugPrint)
        {
            myprintf("Calling Task : ");
            myprintf(_pcName);
            myprintf("\n");
        }
        _pvTaskCode(_task);
    }

    static void *startThread(void *_this)
    {
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
        if (debugPrint)
        {
            SERIAL_DEBUG.print("Start thread ");
            SERIAL_DEBUG.print(static_cast<TaskThread *>(_this)->_pcName);
            SERIAL_DEBUG.print("\n");
        }
        static_cast<TaskThread *>(_this)->_pvTaskCode(static_cast<TaskThread *>(_this));
        if (debugPrint)
        {
            SERIAL_DEBUG.print("End thread ");
            SERIAL_DEBUG.print(static_cast<TaskThread *>(_this)->_pcName);
            SERIAL_DEBUG.print("\n");
        }
        return NULL;
    }
#endif

public:
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
    TaskThread(TaskFunction_t pvTaskCode,
               const char *const pcName,
               const uint32_t usStackDepth = 10000,
               UBaseType_t uxPriority = 5,
               const BaseType_t xCoreID = 0)
        : _pvTaskCode(pvTaskCode), _pcName(pcName)
    {
        if (debugPrint)
        {
            SERIAL_DEBUG.print("Creating Task : ");
            SERIAL_DEBUG.println(_pcName);
        }

#ifdef _VISUAL_STUDIO
        bool ret = pthread_create(&pthread, NULL, startThread, this);
        pthread_setname_np(pthread, pcName);
#else
        /* Task function. */
        /* name of task. */
        /* Stack size of task */
        /* parameter of the task */
        /* priority of the task */
        /* Task handle to keep track of created task */
        /* pin task to core x */
        xTaskCreatePinnedToCore(
            pvTaskCode, pcName, usStackDepth, this, uxPriority, &_task, xCoreID);
#endif
    }

    ~TaskThread()
    {
        if (debugPrint)
        {
            SERIAL_DEBUG.print("Deleting Task : ");
            SERIAL_DEBUG.println(_pcName);
        }
    }

    static void DeleteTask(TaskHandle_t task)
    {
        if (debugPrint)
        {
            SERIAL_DEBUG.print("Deleting Task.");
        }
#ifdef _VISUAL_STUDIO
        pthread_exit(NULL);
#else
        vTaskDelete(task);
#endif
    }
};
#endif
