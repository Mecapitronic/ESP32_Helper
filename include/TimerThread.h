#ifndef TIMER_THREAD_H
#define TIMER_THREAD_H

#ifdef _VISUAL_STUDIO
#include <pthread.h>
typedef void *TimerHandle_t;
typedef void (*TimerCallbackFunction_t)(void *);
#else
#include "ESP32_Helper.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#endif

class TimerThread
{
private:
#ifdef _VISUAL_STUDIO
    pthread_t pthread;
    TimerHandle_t _timer;
    void (*_callBack)(void *);
    const char *_pcName;
    TickType_t _period;
    bool _arret = false;
#else
    TimerHandle_t _timer = nullptr;
    TimerCallbackFunction_t _callBack = nullptr;
    String _pcName = "";
    TickType_t _period = 0;
#endif
    bool _enable = false;
    bool _isRunning = false;

#ifdef _VISUAL_STUDIO
    void timer()
    {
        while (!_arret)
        {
            if (debugPrint)
            {
                myprintf("Calling Timer : ");
                myprintf(_pcName);
                myprintf("\n");
            }
            _callBack(_timer);
            EspClass::timerSleep((double)_period / 1000);
        }
    }

    static void *startTimer(void *_this)
    {
        TimerThread tt;
        tt._callBack = ((TimerThread *)_this)->_callBack;
        tt._pcName = ((TimerThread *)_this)->_pcName;
        tt._period = ((TimerThread *)_this)->_period;
        tt._arret = ((TimerThread *)_this)->_arret;

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
    bool debugPrint = true;
    TimerThread() = default;

    TimerThread(TimerCallbackFunction_t callBack,
                const char *const pcName,
                TickType_t period,
                bool autoReload = true,
                void *const id = 0)
        : _callBack(callBack), _pcName(pcName), _period(period)
    {
        if (debugPrint)
        {
            SERIAL_DEBUG.print("Creating Timer : ");
            SERIAL_DEBUG.println(_pcName);
        }

#ifdef _VISUAL_STUDIO
        bool ret = pthread_create(&pthread, NULL, startTimer, this);
        pthread_setname_np(pthread, pcName);
        _timer = this;
#else
        _timer = xTimerCreate(pcName, period, autoReload, (void *)0, _callBack);
        if (_timer == 0x00)
        {
            if (debugPrint)
            {
                SERIAL_DEBUG.print("Timer ");
                SERIAL_DEBUG.print(_pcName);
                SERIAL_DEBUG.println(" not created");
            }
        }
#endif
    }

    ~TimerThread()
    {
        if (debugPrint)
        {
            SERIAL_DEBUG.print("Deleting Timer : ");
            SERIAL_DEBUG.println(_pcName);
        }
    }

    TickType_t Period()
    {
        return _period;
    }

    void Start()
    {
        if (_timer != NULL)
        {
            if (debugPrint)
            {
                SERIAL_DEBUG.print("Starting Timer : ");
                SERIAL_DEBUG.println(_pcName);
            }
            Enable();
#ifdef _VISUAL_STUDIO
#else
            xTimerStart(_timer, portMAX_DELAY);
#endif
        }
        else
        {
            if (debugPrint)
            {
                SERIAL_DEBUG.print("Timer ");
                SERIAL_DEBUG.print(_pcName);
                SERIAL_DEBUG.println(" not started");
            }
        }
    }

    void Stop()
    {
        if (_timer != NULL)
        {
            if (debugPrint)
            {
                SERIAL_DEBUG.print("Stopping Timer : ");
                SERIAL_DEBUG.println(_pcName);
            }
#ifdef _VISUAL_STUDIO
#else
            xTimerStop(_timer, portMAX_DELAY);
#endif
        }
        else
        {
            if (debugPrint)
            {
                SERIAL_DEBUG.print("Timer ");
                SERIAL_DEBUG.print(_pcName);
                SERIAL_DEBUG.println(" not stopped");
            }
        }
    }

    void WaitForDisable()
    {
        if (debugPrint)
        {
            SERIAL_DEBUG.print("Timer OFF : ");
            SERIAL_DEBUG.println(_pcName);
        }
        _enable = false;
        if (_isRunning)
        {
            if (debugPrint)
            {
                SERIAL_DEBUG.print("Wait Timer stop running : ");
                SERIAL_DEBUG.println(_pcName);
            }
        }
        while (_isRunning)
        {
            vTaskDelay(1);
            // Wait for the timer to be disabled by callback at the end of the callback
        }
    }

    void Enable()
    {
        if (debugPrint)
        {
            SERIAL_DEBUG.print("Timer ON : ");
            SERIAL_DEBUG.println(_pcName);
        }
        _enable = true;
    }

    bool IsEnable()
    {
        return _enable == true;
    }

    bool Running(bool _run)
    {
        return _isRunning = _run;
    }
};
#endif
