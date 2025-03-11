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
    TaskHandle_t _task;
    //TaskFunction_t _pvTaskCode;
    String _pcName;
    void (*_pvTaskCode) (void*);
    
    public:
    TaskThread(){}

    TaskThread( TaskFunction_t pvTaskCode,
        const char * const pcName,
        const uint32_t usStackDepth = 10000,
        UBaseType_t uxPriority = 5,
        const BaseType_t xCoreID = 0)
    {
        _pvTaskCode = pvTaskCode;
        _pcName = String(pcName);
        
        Serial.println("Creating Task : ");
        Serial.println(_pcName);

        /* Task function. */
        /* name of task. */
        /* Stack size of task */
        /* parameter of the task */
        /* priority of the task */
        /* Task handle to keep track of created task */
        /* pin task to core x */
        xTaskCreatePinnedToCore(this->startTaskImpl, pcName, usStackDepth, this, uxPriority, &_task, xCoreID);
    }

    ~TaskThread()
    {
        Serial.println("Deleting Task : ");
        Serial.println(_pcName);
        //vTaskDelete(_task);
    }

    private:
    
    static void startTaskImpl(void * _this)
    {       
        Serial.println("Impl Task : ");
        Serial.println(((TaskThread*)_this)->_pcName); 
        ((TaskThread*)_this)->task();
    }

    void task()
    {
        Serial.println("Calling Task : ");
        Serial.println(_pcName);
        _pvTaskCode(NULL);
    }

    //get stack for task 2
    //unsigned int temp2 = uxTaskGetStackHighWaterMark(nullptr);
    //Serial.print("task2="); Serial.println(temp2);
    //log_i( "fDoTheHumidityThing high watermark %d",  uxTaskGetStackHighWaterMark( NULL ) );

};

#endif