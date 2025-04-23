#ifndef TIMER_THREAD_H
#define TIMER_THREAD_H

#ifdef _VISUAL_STUDIO
#include <pthread.h>
typedef void *TimerHandle_t;
typedef void (*TimerCallbackFunction_t)(void*);
#else
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#endif

class TimerThread
{
private:
    TimerHandle_t _timer;
    TimerCallbackFunction_t _callBack;
    String _pcName;
    TickType_t _period;

public:
    TimerThread() {}
    TimerThread(TimerCallbackFunction_t callBack, const char* const pcName, TickType_t period, bool autoReload = true, void * const id = 0)
    {
        _callBack = callBack;
        _pcName = pcName;
        _period = period;

        SERIAL_DEBUG.print("Creating Timer : ");
        SERIAL_DEBUG.println(_pcName);
        _timer = xTimerCreate(pcName, period, autoReload, (void *)0, callBack);
    }
    
    ~TimerThread()
    {
        SERIAL_DEBUG.print("Deleting Timer : ");
        SERIAL_DEBUG.println(_pcName);
        //xTimerDelete(_timer);
        SERIAL_DEBUG.println("Deleted timer");
    }
    
    TickType_t Period()
    {
        return _period;
    }

    void Start()
    {
        while(_timer==0x00){}
        SERIAL_DEBUG.print("Starting Timer : ");
        SERIAL_DEBUG.println(_pcName);
        xTimerStart(_timer, portMAX_DELAY);
    }

    void Stop()
    {
        SERIAL_DEBUG.print("Stopping Timer : ");
        SERIAL_DEBUG.println(_pcName);
        xTimerStop(_timer, portMAX_DELAY);
    }
};
#endif
