#ifndef QUEUE_THREAD_H
#define QUEUE_THREAD_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

template <typename T>
class QueueThread
{
    private:
    QueueHandle_t _queue;
    bool isInit = false;

    public:
    QueueThread(){Serial.println("Create queue : ");}

    QueueThread(int size)
    {
        Serial.println("Creating queue : ");

        _queue = xQueueCreate(size, sizeof(T));
        if(_queue == nullptr)
        {
            Serial.println("Init queue NOK");
            isInit = false;
        }
        else
        {
            Serial.println("Init queue OK");
            isInit = true;
        }
    }

    ~QueueThread()
    {
        Serial.println("Deleting queue");
        if (isInit)
        {
            //vQueueDelete(_queue);
            Serial.println("Deleted queue");
        }
    }

    void Delete()
    {
        this->~QueueThread();
    }

    bool IsInit()
    {
        return isInit;
    }

    void Send(const T& item)
    {
        if (isInit)
        {
            Serial.println("Sending to queue");
            xQueueSend(_queue, &item, 0);

        }        
    }

    bool Receive(T& item)
    {
        if (isInit)
        {
            Serial.println("Receive from queue");
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