#ifndef TIMER_THREAD_H
#define TIMER_THREAD_H

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "ESP32_Helper.h"

class TimerThread
{
private:
    TimerHandle_t _timer;
    TimerCallbackFunction_t _callBack;
    String _pcName;
    TickType_t _period;

public:
    bool debugPrint = false;
    TimerThread() = default;
    TimerThread(TimerCallbackFunction_t callBack, const char *const pcName, TickType_t period, bool autoReload = true, void *const id = 0)
        : _callBack(callBack), _pcName(pcName), _period(period)
    {
        if (debugPrint)
        {
            SERIAL_DEBUG.print("Creating Timer : ");
            SERIAL_DEBUG.println(_pcName);
        }

        _timer = xTimerCreate(pcName, period, autoReload, (void *)0, callBack);
    }

    ~TimerThread()
    {
        if (debugPrint)
        {
            SERIAL_DEBUG.print("Deleting Timer : ");
            SERIAL_DEBUG.println(_pcName);
            SERIAL_DEBUG.println("Deleted timer");
        }
    }

    TickType_t Period()
    {
        return _period;
    }

    void Start()
    {
        if (debugPrint)
        {
            SERIAL_DEBUG.print("Starting Timer : ");
            SERIAL_DEBUG.println(_pcName);
        }
        while (_timer == 0x00)
        {
        }
        xTimerStart(_timer, portMAX_DELAY);
    }

    void Stop()
    {
        if (debugPrint)
        {
            SERIAL_DEBUG.print("Stopping Timer : ");
            SERIAL_DEBUG.println(_pcName);
        }
        xTimerStop(_timer, portMAX_DELAY);
    }
};
#endif
