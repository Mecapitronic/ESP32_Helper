#ifndef TIMER_THREAD_H
#define TIMER_THREAD_H

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "ESP32_Helper.h"

class TimerThread
{
private:
    TimerHandle_t _timer = nullptr;
    TimerCallbackFunction_t _callBack = nullptr;
    String _pcName = "";
    TickType_t _period = 0;
    bool _enable = false;
    bool _isRunning = false;

public:
    bool debugPrint = true;
    TimerThread() = default;
    TimerThread(TimerCallbackFunction_t callBack, const char *const pcName, TickType_t period, bool autoReload = true, void *const id = 0)
        : _callBack(callBack), _pcName(pcName), _period(period)
    {
        if (debugPrint)
        {
            SERIAL_DEBUG.print("Creating Timer : ");
            SERIAL_DEBUG.println(_pcName);
        }

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
            xTimerStart(_timer, portMAX_DELAY);
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
            xTimerStop(_timer, portMAX_DELAY);
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
