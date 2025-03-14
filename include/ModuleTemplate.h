#ifndef MODULE_TEMPLATE_H
#define MODULE_TEMPLATE_H

#include "ESP32_Helper.h"
using namespace Printer;

//// template <class T>
class IModule
{
private:
    // QueueHandle_t myQueue;
    // static const int queueSize = 500;

public:
    // This function will be be called just before the constructor of the module
    IModule(void) { println("Creating new module with IModule Interface"); }

    // These bases functions() = 0 NEED to be implemented by derived class
    virtual void Initialisation() = 0;
    virtual void Update() = 0;
    virtual void HandleCommand(Command cmd) = 0;
    virtual void PrintCommandHelp() = 0;

    // These bases functions() {} will be called only if not implemented by derived class
    virtual ~IModule() { println("Deleting IModule"); }
    virtual void SetConfig() { println("SetConfig IModule"); }
    // virtual bool ReadSerial() { println("ReadSerial IModule : not implemented !"); }
    // virtual void SendSerial() { println("SendSerial IModule : not implemented !"); }

    // TEST ONLY
    /*
    void CreateQueue(int size = queueSize)
    {
        myQueue = xQueueCreate(size, sizeof(uint8_t));
        if (myQueue == NULL)
        {
            //Printer::println("Error creating the queue", LEVEL_ERROR);
        }
    }
    */
};
#endif
