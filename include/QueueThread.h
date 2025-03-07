#ifndef QUEUE_THREAD_H
#define QUEUE_THREAD_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

template <typename T>
class QueueThread
{
    private:
    QueueHandle_t queue;
    bool isInit = false;

    public:
    QueueThread(){}

    QueueThread(int size)
    {
        queue = xQueueCreate(size, sizeof(T));
        if(queue == nullptr)
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
            vQueueDelete(queue);
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
            xQueueSend(queue, &item, 0);
        }        
    }

    bool Receive(T& item)
    {
        if (isInit)
        {
            return xQueueReceive(queue, &item, 0);
        }
        return false;
    }

    int MessagesWaiting()
    {
        if (isInit)
        {
            return uxQueueMessagesWaiting(queue);
        }
        return 0;
    }
};

#endif