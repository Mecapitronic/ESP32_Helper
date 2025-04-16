#ifndef MODULE_TEMPLATE_H
#define MODULE_TEMPLATE_H

#include "ESP32_Helper.h"

//// template <class T>
class IModule
{
private:
    /* data */

public:
    // This function will be be called just before the constructor of the module
    IModule(void) { Printer::println("Creating new module with IModule Interface"); }

    // These bases functions() = 0 NEED to be implemented by derived class
    virtual void Initialisation() = 0;
    virtual void Update() = 0;
    virtual void HandleCommand(Command cmd) = 0;
    virtual void PrintCommandHelp() = 0;

    // These bases functions() {} will be called only if not implemented by derived class
    virtual ~IModule() { Printer::println("Deleting IModule"); }
    virtual void SetConfig() { Printer::println("SetConfig IModule"); }
    // virtual bool ReadSerial() { Printer::println("ReadSerial IModule : not implemented !"); }
    // virtual void SendSerial() { Printer::println("SendSerial IModule : not implemented !"); }

};
#endif
