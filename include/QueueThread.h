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

public:
    bool debugPrint = false;
    QueueThread() = default;
    QueueThread(int32_t size)
    {
        if (debugPrint)
            SERIAL_DEBUG.println("Creating Queue : ");
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
            xQueueSend(_queue, &item, 0);
        }
    }

    bool Receive(T &item)
    {
        if (isInit)
        {
            if (debugPrint)
                SERIAL_DEBUG.println("Receive from Queue");
            return xQueueReceive(_queue, &item, 0);
        }
        return false;
    }

    int32_t MessagesWaiting()
    {
        if (isInit)
        {
            int32_t msgWait = uxQueueMessagesWaiting(_queue);
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
