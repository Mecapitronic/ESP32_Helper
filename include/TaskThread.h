#ifndef TASK_THREAD_H
#define TASK_THREAD_H

#ifdef _VISUAL_STUDIO
#include <pthread.h>
#define portBASE_TYPE int
typedef portBASE_TYPE BaseType_t;
typedef unsigned portBASE_TYPE UBaseType_t;
typedef void (*TaskFunction_t)(void*);
#else
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

class TaskThread
{
    private:
#ifdef _VISUAL_STUDIO
     pthread_t pthread;
     // TaskFunction_t _pvTaskCode;
     void (*_pvTaskCode)(void*);
     const char* _pcName;
#else
     TaskHandle_t _task;
     TaskFunction_t _pvTaskCode;
     String _pcName;
#endif

     void task()
     {
         Serial.print("Calling Task : ");
         Serial.println(_pcName);
         _pvTaskCode(NULL);
     }

     static void startTaskImpl(void* _this)
     {
         Serial.print("Impl Task : ");
         Serial.println(((TaskThread*)_this)->_pcName);
         ((TaskThread*)_this)->task();
     }

     static void* startThread(void* _this)
     {
         ((TaskThread*)_this)->task();
         return NULL;
     }

    public:
     TaskThread() {}

     TaskThread(TaskFunction_t pvTaskCode, const char* const pcName, const uint32_t usStackDepth = 10000,
                UBaseType_t uxPriority = 5, const BaseType_t xCoreID = 0)
     {
         _pvTaskCode = pvTaskCode;
         _pcName = pcName;

         Serial.print("Creating Task : ");
         Serial.println(_pcName);

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
         xTaskCreatePinnedToCore(this->startTaskImpl, pcName, usStackDepth, this, uxPriority, &_task, xCoreID);
#endif
     }

     ~TaskThread()
     {
         Serial.print("Deleting Task : ");
         Serial.println(_pcName);
#ifdef _VISUAL_STUDIO
         // bool cancel = pthread_cancel(pthread);
         // bool join = pthread_join(pthread, NULL);
#else
         // vTaskDelete(_task);
         Serial.println("Deleted task");
#endif
     }

     // get stack for task 2
     // unsigned int32_t temp2 = uxTaskGetStackHighWaterMark(nullptr);
     // Serial.print("task2="); Serial.println(temp2);
     // log_i( "fDoTheHumidityThing high watermark %d",  uxTaskGetStackHighWaterMark( NULL ) );
};
#endif
