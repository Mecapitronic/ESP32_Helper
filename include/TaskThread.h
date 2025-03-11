#ifndef TASK_THREAD_H
#define TASK_THREAD_H

#ifndef _VISUAL_STUDIO
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

//template <typename T>
class TaskThread
{
    private:
    TaskHandle_t task;

    public:
    TaskThread(){}

    TaskThread( TaskFunction_t pvTaskCode,
        const char * const pcName,
        const uint32_t usStackDepth = 10000,
        void * const pvParameters = NULL,
        UBaseType_t uxPriority = 5,
        const BaseType_t xCoreID = 0)
    {
        /* Task function. */
        /* name of task. */
        /* Stack size of task */
        /* parameter of the task */
        /* priority of the task */
        /* Task handle to keep track of created task */
        /* pin task to core x */
        xTaskCreatePinnedToCore(pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, &task, xCoreID);
    }

    ~TaskThread()
    {
        vTaskDelete(task);
    }

    void Start()
    {
        
    //get stack for task 2
    //unsigned int temp2 = uxTaskGetStackHighWaterMark(nullptr);
    //Serial.print("task2="); Serial.println(temp2);
        
    //log_i( "fDoTheHumidityThing high watermark %d",  uxTaskGetStackHighWaterMark( NULL ) );
        vTaskResume(task);
    }
};

#endif