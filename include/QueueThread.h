#ifndef QUEUE_THREAD_H
#define QUEUE_THREAD_H

#ifdef _VISUAL_STUDIO
#include <pthread.h>
#define QueueHandle_t vector<T>
#else
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#endif
#include "ESP32_Helper.h"
#include "TaskThread.h" 

template <typename T> class QueueThread
{
private:
    QueueHandle_t _queue;
    bool isInit = false;

public:
    QueueThread() = default;

    QueueThread(int32_t size)
    {
        if (debugPrint)
            SERIAL_DEBUG.println("Creating Queue : ");
#ifdef _VISUAL_STUDIO
        _queue.reserve(size);
        if (debugPrint)
            SERIAL_DEBUG.println("Init queue OK");
        isInit = true;
#else
        _queue = xQueueCreate(size, sizeof(T));
        if (_queue == nullptr)
        {
            if (debugPrint)
                SERIAL_DEBUG.println("Init Queue NOK");
            isInit = false;
        }
        else
        {
            if (debugPrint)
                SERIAL_DEBUG.println("Init Queue OK");
            isInit = true;
        }
#endif
    }

    ~QueueThread()
    {
        if (debugPrint)
            SERIAL_DEBUG.println("Deleting Queue");
        if (isInit)
        {
            if (debugPrint)
                SERIAL_DEBUG.println("Deleted Queue");
        }
    }

    bool IsInit()
    {
        return isInit;
    }

    void Send(const T &item)
    {
        if (isInit)
        {
            if (debugPrint)
                SERIAL_DEBUG.println("Sending to Queue");
#ifdef _VISUAL_STUDIO
            _queue.push_back(item);
#else
            xQueueSend(_queue, &item, 0);
#endif
        }
    }

    bool Receive(T &item)
    {
        if (isInit)
        {
            if (debugPrint)
                SERIAL_DEBUG.println("Receive from Queue");
#ifdef _VISUAL_STUDIO
            item = _queue.at(0);
            pop_front(_queue);
            return true;
#else
            return xQueueReceive(_queue, &item, 0);
#endif
        }
        return false;
    }

    int32_t MessagesWaiting()
    {
        if (isInit)
        {
#ifdef _VISUAL_STUDIO
            int32_t msgWait = _queue.size();
#else
            int32_t msgWait = uxQueueMessagesWaiting(_queue);
#endif
            if (debugPrint)
            {
                SERIAL_DEBUG.print("Waiting in Queue : ");
                SERIAL_DEBUG.println(msgWait);
            }
            return msgWait;
        }
        return 0;
    }
};

#endif
