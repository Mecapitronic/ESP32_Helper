#ifndef QUEUE_THREAD_H
#define QUEUE_THREAD_H

#ifndef _VISUAL_STUDIO
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#endif

template <typename T>
class QueueThread
{
    private:
    QueueHandle_t _queue;
    bool isInit = false;

    public:
    QueueThread(){}

    QueueThread(int size)
    {
        _queue = xQueueCreate(size, sizeof(T));
        if(_queue == nullptr)
        {
            isInit = false;
        }
        else
        {
            isInit = true;
        }
    }

    ~QueueThread()
    {
        if (isInit)
        {
            vQueueDelete(_queue);
        }
    }

    bool IsInit()
    {
        return isInit;
    }

    void Send(const T& item)
    {
        if (isInit)
        {
            xQueueSend(_queue, &item, 0);
        }        
    }

    bool Receive(T& item)
    {
        if (isInit)
        {
            return xQueueReceive(_queue, &item, 0);
        }
        return false;
    }

    int MessagesWaiting()
    {
        if (isInit)
        {
            return uxQueueMessagesWaiting(_queue);
        }
        return 0;
    }
};

#endif