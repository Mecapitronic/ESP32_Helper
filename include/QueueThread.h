#ifndef QUEUE_THREAD_H
#define QUEUE_THREAD_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "ESP32_Helper.h"

template <typename T>
class QueueThread
{
    private:
    QueueHandle_t _queue;
    bool isInit = false;
    bool debug = false;

    public:
    QueueThread()
    {
        if (debug)
            SERIAL_DEBUG.println("Create queue : ");
    }

    QueueThread(int32_t size)
    {
        if (debug)
            SERIAL_DEBUG.println("Creating queue : ");
        _queue = xQueueCreate(size, sizeof(T));
        if(_queue == nullptr)
        {
            if (debug)
                SERIAL_DEBUG.println("Init queue NOK");
            isInit = false;
        }
        else
        {
            if (debug)
                SERIAL_DEBUG.println("Init queue OK");
            isInit = true;
        }
    }

    ~QueueThread()
    {
        if (debug)
            SERIAL_DEBUG.println("Deleting queue");
        if (isInit)
        {
            if (debug)
                SERIAL_DEBUG.println("Deleted queue");
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
            if (debug)
                SERIAL_DEBUG.println("Sending to queue");
            xQueueSend(_queue, &item, 0);

        }        
    }

    bool Receive(T& item)
    {
        if (isInit)
        {
            if (debug)
                SERIAL_DEBUG.println("Receive from queue");
            return xQueueReceive(_queue, &item, 0);
        }
        return false;
    }

    int32_t MessagesWaiting()
    {
        if (isInit)
        {
            return uxQueueMessagesWaiting(_queue);
        }
        return 0;
    }
};

#endif
