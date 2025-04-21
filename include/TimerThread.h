#ifndef TIMER_THREAD_H
#define TIMER_THREAD_H

#ifdef _VISUAL_STUDIO
#include <pthread.h>
typedef void *TimerHandle_t;
typedef void (*TimerCallbackFunction_t)(void*);
#else
#include "timers.h"
#endif

class TimerThread
{
private:
#ifdef _VISUAL_STUDIO
pthread_t pthread;
TimerHandle_t _timer;
void (*_callBack)(void*);
const char* _pcName;
TickType_t _period;
bool _arret = false;
#else
    TimerHandle_t _timer;
    TimerCallbackFunction_t _callBack;
    String _pcName;
    TickType_t _period;
#endif


#ifdef _VISUAL_STUDIO
    void timer()
    {
        while (!_arret)
        {
            _callBack(_timer);
            EspClass::timerSleep((double)_period / 1000);
        }
    }
    static void* startTimer(void* _this)
    {
        TimerThread tt;
        tt._callBack = ((TimerThread*)_this)->_callBack;
        tt._pcName = ((TimerThread*)_this)->_pcName;
        tt._period = ((TimerThread*)_this)->_period;
        tt._arret = ((TimerThread*)_this)->_arret;

        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
        myprintf("Start timer ");
        myprintf(tt._pcName);
        myprintf("\n");
        tt.timer();
        myprintf("End timer ");
        myprintf(tt._pcName);
        myprintf("\n");
        return NULL;
    }
#endif

public:
    TimerThread() {}
    TimerThread(TimerCallbackFunction_t callBack, const char* const pcName, TickType_t period, bool autoReload = true, void * const id = 0)
    {
        _callBack = callBack;
        _pcName = pcName;
        _period = period;

        Serial.print("Creating Timer : ");
        Serial.println(_pcName);
        
#ifdef _VISUAL_STUDIO
    bool ret = pthread_create(&pthread, NULL, startTimer, this);
    pthread_setname_np(pthread, pcName);
#else
_timer = xTimerCreate(pcName, period, autoReload, (void *)0, callBack);
#endif
    }
    
    ~TimerThread()
    {
        Serial.print("Deleting Timer : ");
        Serial.println(_pcName);
#ifdef _VISUAL_STUDIO
        // bool cancel = pthread_cancel(pthread);
        // bool join = pthread_join(pthread, NULL);
#else
        //xTimerDelete(_timer);
        Serial.println("Deleted timer");
#endif
    }
    
    TickType_t Period()
    {
        return _period;
    }

    void Start()
    {
        while(_timer==0x00){}
        Serial.print("Starting Timer : ");
        Serial.println(_pcName);
        #ifdef _VISUAL_STUDIO
        #else
        xTimerStart(_timer, portMAX_DELAY);
        #endif
    }

    void Stop()
    {
        Serial.print("Stopping Timer : ");
        Serial.println(_pcName);
        #ifdef _VISUAL_STUDIO
        #else
        xTimerStop(_timer, portMAX_DELAY);
        #endif

    }
};
#endif