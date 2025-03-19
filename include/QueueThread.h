#ifndef QUEUE_THREAD_H
#define QUEUE_THREAD_H

#ifdef _VISUAL_STUDIO
#include <pthread.h>
#define QueueHandle_t vector<T>
#else
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
    QueueThread(){Serial.println("Create queue : ");}

    QueueThread(int32_t size)
    {
        Serial.println("Creating queue : ");
#ifdef _VISUAL_STUDIO
        _queue.reserve(size);
        Serial.println("Init queue OK");
        isInit = true;
#else
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
#endif
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
#ifdef _VISUAL_STUDIO
            _queue.push_back(item);
#else
            xQueueSend(_queue, &item, 0);
#endif
        }        
    }

    bool Receive(T& item)
    {
        if (isInit)
        {
            Serial.println("Receive from queue");
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
            return _queue.size();
#else
            return uxQueueMessagesWaiting(_queue);
#endif
        }
        return 0;
    }
};

#endif
