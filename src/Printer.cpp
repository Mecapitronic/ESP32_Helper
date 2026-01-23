#include "Printer.h"
#include <cstdarg>

namespace Printer
{
    Teleplot teleplotUDP;
    Enable teleplotUDPEnable = Enable::ENABLE_FALSE;
    namespace
    {
        Level printLevel = Level::LEVEL_VERBOSE;
        Enable printEnable = Enable::ENABLE_TRUE;
    }

    void SetLevel(Level level)
    {
        printLevel = level;
        print("Printer Level : ");
        switch (level)
        {
            ENUM_PRINT(Level::LEVEL_VERBOSE);
            ENUM_PRINT(Level::LEVEL_INFO);
            ENUM_PRINT(Level::LEVEL_WARN);
            ENUM_PRINT(Level::LEVEL_ERROR);
            ENUM_PRINT(Level::LEVEL_NONE);
        }
    }

    void EnablePrinter(Enable enable)
    {
        if (enable != Enable::ENABLE_NONE)
        {
            print("Printer : ");

            if (printEnable == enable && enable == Enable::ENABLE_TRUE)
            {
                print("already Enable");
            }
            if (printEnable == enable && enable == Enable::ENABLE_FALSE)
            {
                print("already Disable");
            }
            if (printEnable != enable && enable == Enable::ENABLE_TRUE)
            {
                print(" Enable");
            }
            if (printEnable != enable && enable == Enable::ENABLE_FALSE)
            {
                print(" Disable");
            }
            println();
            printEnable = enable;
        }
    }

    bool IsEnable() { return printEnable == Enable::ENABLE_TRUE; }

    bool IsPrintable(Level level) { return printEnable == Enable::ENABLE_TRUE && printLevel <= level; }

    void Initialisation(Enable enable, Level lvl)
    {
        SetLevel(lvl);
        EnablePrinter(enable);
    }
    
    void Update(void *pvParameters)
    {
        println("Printer Update");
    }

    void HandleCommand(Command cmdTmp)
    {
        if (cmdTmp.cmd == "PrintLevel")
        {
            // PrintLevel:0
            if (cmdTmp.size == 1)
                SetLevel((Level)cmdTmp.data[0]);
        }
        else if (cmdTmp.cmd == "PrintEnable")
        {
            // PrintEnable:1
            if (cmdTmp.size == 1)
                EnablePrinter((Enable)cmdTmp.data[0]);
        }
        else if (cmdTmp.cmd == "PrintTeleplotUDP" && cmdTmp.size == 1)
        {
            // PrintTeleplotUDP:0
            if (cmdTmp.data[0] == 0)
            {
                teleplotUDPEnable = Enable::ENABLE_FALSE;
                println("Teleplot UDP Disable");
            }
            else
            {
                teleplotUDPEnable = Enable::ENABLE_TRUE;
                println("Teleplot UDP Enable");
            }
        }
        else
        {
            println("Not a Print command !");            
            PrintCommandHelp();
        }
    }

    void PrintCommandHelp()
    {
        println("Printer Command Help :");
        println(" > PrintLevel:[int]");
        println("      0 VERBOSE, 1 INFO, 2 WARN, 3 ERROR");
        println(" > PrintEnable:[int]");
        println("      0 Disable, 1 Enable Debugger");
        println(" > PrintTeleplotUDP:[int]");
        println("      0 Disable, 1 Enable");
        println();
    }

    Level PrintLevel() { return printLevel; }
    void PrintLevel(Level level)
    {
        printLevel = level;
        print("PrintLevel : ");
        switch (level)
        {
            ENUM_PRINT(Level::LEVEL_VERBOSE);
            ENUM_PRINT(Level::LEVEL_INFO);
            ENUM_PRINT(Level::LEVEL_WARN);
            ENUM_PRINT(Level::LEVEL_ERROR);
            ENUM_PRINT(Level::LEVEL_NONE);
        }
    }

    Enable PrintEnable() { return printEnable; }
    void PrintEnable(Enable enable)
    {
        printEnable = enable;
        print("PrintEnable : ");
        switch (enable)
        {
            ENUM_PRINT(Enable::ENABLE_FALSE);
            ENUM_PRINT(Enable::ENABLE_TRUE);
            ENUM_PRINT(Enable::ENABLE_NONE);
        }
    }

    /**
Format Specifier
    %c    For character type.
    %d    For signed integer type.
    %e or %E    For scientific notation of floats.
    %f    For float type.
    %g or %G    For float type with the current precision.
    %i    signed integer
    %ld or %li    Long
    %lf    Double
    %Lf    Long double
    %lu    Unsigned int or unsigned long
    %lli or %lld    Long long
    %llu    Unsigned long long
    %o    Octal representation
    %p    Pointer
    %s    String  /!\ DOES NOT WORK WITH ARDUINO String OBJECT /!\
    %u    Unsigned int
    %x or %X    Hexadecimal representation
    %n    Prints nothing
    %%    Prints % character
    **/
    void print(const char *fmt, ...)
    {
        if (!IsEnable())
            return;
        if (!fmt)
            fmt = "";
        va_list args;
        va_start(args, fmt);
        SERIAL_DEBUG.vprintf(fmt, args);
        if (Wifi_Helper::IsEnable())
            WIFI_DEBUG.vprintf(fmt, args);
        va_end(args);
    }

    void println(const char *fmt, ...)
    {
        if (!IsEnable())
            return;
        if (!fmt)
            fmt = "";
        va_list args;
        va_start(args, fmt);
        SERIAL_DEBUG.vprintf(fmt, args);
        if (Wifi_Helper::IsEnable())
            WIFI_DEBUG.vprintf(fmt, args);
        va_end(args);
        
        SERIAL_DEBUG.println();
        if (Wifi_Helper::IsEnable())
            WIFI_DEBUG.println();
    }

    void println()
    {
        if (!IsEnable())
            return;
        SERIAL_DEBUG.println();
        if (Wifi_Helper::IsEnable())
            WIFI_DEBUG.println();
    }
        
    void print(const String &str)
    {
        if (!IsEnable())
            return;
        SERIAL_DEBUG.print(str);
        if (Wifi_Helper::IsEnable())
            WIFI_DEBUG.print(str);
    }

    void println(const String &str)
    {
        print(str);
        println();
    }

    void printError(const String &error)
    {
        println("!!!---------- ERROR ----------!!!");
        println(error);
        println("!!!---------- ERROR ----------!!!");
    }

    void printChrono(Chrono chrono)
    {
        println("Chrono ["+chrono.name+"]: " + String(chrono.elapsedTime) + " µs");
    }

    void teleplot(const String &varName, float var)
    {
        if (teleplotUDP.IsInitialized())
            teleplotUDP.update(varName.c_str(), var);
        else
            println(">" + varName + ":" + String(var));
    }

    void teleplot(const String &varName, Point point)
    {
        if (teleplotUDP.IsInitialized())
            teleplotUDP.update2D(varName.c_str(), point.x, point.y);
        else
            println(">" + varName + ":" + String((int)point.x) + ":" + String((int)point.y) + "|xy");
    }

/*
    void teleplot(const String &varName, PolarPoint point, int32_t timeStamp)
    {
        String data = ">" + varName + ":" + point.x + ":" + point.y + ":" + timeStamp + "|xy";
        println(data);
    }
*/
/*
    void teleplot(const String &varName, PolarPoint polarPoints[], uint16_t size)
    {
        String data = ">" + varName + ":";
        for (uint16_t i = 0; i < size; i++)
        {
            data += String() + (int)(polarPoints[i].x) + ":" + (int)(polarPoints[i].y) + ";";
        }
        data += "|xy";
        println(data);
    }
*/
/*
    void teleplot(const String &varName, vector<PolarPoint> vec)
    {
        // if (vec.size() == 0)
        //     return;
        print(">" + varName + ":");
        for (auto &v : vec)
        {
            print(String() + (int)v.x + ":" + (int)v.y + ";");
        }
        println("|xy");
    }
*/
/*
    void teleplot(const String &varName, Point4D point)
    {
        String data = ">" + varName + ":" + (int)point.x + ":" + (int)point.y + "|xy";
        println(data);
    }
*/
/*
    void plot3D(const String &varName, Point3D p)
    {
        // 3D|A:B:C|E
        // '3D|sphere1,widget0:S:sphere:RA:'+ str(sphere1rad)+':P:'+ str(sphere1x) +':'+ str(sphere1y) +':'+
        // str(sphere1z) + ':C:black:O:1' msg = '3D|sphere' + sphere1num + ',widget0:S:sphere:RA:' + str(sphere1rad) +
        // ':P:' + str(sphere1x) + ':' + str(sphere1y) + ':' + str(sphere1z)
        // + ':C:black:O:1' #2ecc71
        // TODO : rester en cm sinon le point est perdu dans le brouillard de l'horizon 3D de teleplot...
        // FIXME : la couleur n'est pas prise en compte
        // FIXME : le repère 3D de teleplot est basé sur un plan XZ avec Y en "hauteur" (sans axe!)
        String data =
            ">3D|" + varName + ",widget0:S:sphere:P:" + p.x + ":" + p.y + ":" + p.z / 10 + ":RA:1:C:black:O:1";
        println(data);
    }
*/
/*
    void plot3Dpy(Point3D p)
    {
        String data = "" + String(p.x) + ":" + String(p.y) + ":" + String(p.z);
        println(data);
    }
*/
}
