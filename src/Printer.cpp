#include "Printer.h"
#include "Logger.h"
#include <cstdarg>

namespace Printer
{
    Teleplot teleplotUDP;
    Enable teleplotUDPEnable = Enable::ENABLE_FALSE;
    Enable teleplotEnable = Enable::ENABLE_FALSE;
    namespace
    {
        Level printLevel = Level::LEVEL_VERBOSE;
        Enable printEnable = Enable::ENABLE_TRUE;
    }

    void SetLevel(Level level)
    {
        printLevel = level;
        const char *levelName = "LEVEL_UNKNOWN";
        switch (level)
        {
            case Level::LEVEL_VERBOSE:
                levelName = "LEVEL_VERBOSE";
                break;
            case Level::LEVEL_INFO:
                levelName = "LEVEL_INFO";
                break;
            case Level::LEVEL_WARN:
                levelName = "LEVEL_WARN";
                break;
            case Level::LEVEL_ERROR:
                levelName = "LEVEL_ERROR";
                break;
            case Level::LEVEL_NONE:
                levelName = "LEVEL_NONE";
                break;
            default:
                break;
        }
        println("Printer Level %s", levelName);
    }

    void EnablePrinter(Enable enable)
    {
        if (enable != Enable::ENABLE_NONE)
        {
            const char *status = "unknown";

            if (printEnable == enable && enable == Enable::ENABLE_TRUE)
            {
                status = "already Enable";
            }
            if (printEnable == enable && enable == Enable::ENABLE_FALSE)
            {
                status = "already Disable";
            }
            if (printEnable != enable && enable == Enable::ENABLE_TRUE)
            {
                status = "Enable";
            }
            if (printEnable != enable && enable == Enable::ENABLE_FALSE)
            {
                status = "Disable";
            }
            println("Printer %s", status);
            printEnable = enable;
        }
    }

    bool IsEnable() { return printEnable == Enable::ENABLE_TRUE; }

    bool IsPrintable(Level level) { return printEnable == Enable::ENABLE_TRUE && printLevel <= level; }

    void Initialisation(Enable enable, Level lvl)
    {
        SetLevel(lvl);
        EnablePrinter(enable);
        RegisterCommands();
    }

    void RegisterCommands()
    {
        ESP32_Helper::RegisterCommand("PrintLevel", "PrintLevel:<level:int> - Set print level; 0=verbose, 1=info, 2=warn, 3=error, 4=none.", FunctionCommand( SetLevel((Level)cmd.data[0]); ));
        ESP32_Helper::RegisterCommand("PrintEnable", "PrintEnable:<enabled:bool> - Enable or disable printing.", FunctionCommand( EnablePrinter((Enable)cmd.data[0]); ));
        ESP32_Helper::RegisterCommand("PrintTeleplot", "PrintTeleplot:<enabled:bool> - Enable or disable serial Teleplot output.", FunctionCommand( teleplotEnable = (cmd.data[0] == 0 ? Enable::ENABLE_FALSE : Enable::ENABLE_TRUE); ));
        ESP32_Helper::RegisterCommand("PrintTeleplotUDP", "PrintTeleplotUDP:<enabled:bool> - Enable or disable UDP Teleplot output.", FunctionCommand( teleplotUDPEnable = (cmd.data[0] == 0 ? Enable::ENABLE_FALSE : Enable::ENABLE_TRUE); ));
        ESP32_Helper::RegisterCommand("PrintChrono", "PrintChrono:<enabled:bool> - Enable or disable timing output.", FunctionCommand( Chrono::print = (cmd.data[0] == 0 ? false : true); ));
    }
    
    void Update(void *pvParameters)
    {
        println("Printer Update");
    }

    Level PrintLevel() { return printLevel; }
    void PrintLevel(Level level)
    {
        printLevel = level;
        const char *levelName = "LEVEL_UNKNOWN";
        switch (level)
        {
            case Level::LEVEL_VERBOSE:
                levelName = "LEVEL_VERBOSE";
                break;
            case Level::LEVEL_INFO:
                levelName = "LEVEL_INFO";
                break;
            case Level::LEVEL_WARN:
                levelName = "LEVEL_WARN";
                break;
            case Level::LEVEL_ERROR:
                levelName = "LEVEL_ERROR";
                break;
            case Level::LEVEL_NONE:
                levelName = "LEVEL_NONE";
                break;
            default:
                break;
        }
        println("PrintLevel %s", levelName);
    }

    Enable PrintEnable() { return printEnable; }
    void PrintEnable(Enable enable)
    {
        printEnable = enable;
        const char *enableName = "ENABLE_UNKNOWN";
        switch (enable)
        {
            case Enable::ENABLE_FALSE:
                enableName = "ENABLE_FALSE";
                break;
            case Enable::ENABLE_TRUE:
                enableName = "ENABLE_TRUE";
                break;
            case Enable::ENABLE_NONE:
                enableName = "ENABLE_NONE";
                break;
            default:
                break;
        }
        println("PrintEnable %s", enableName);
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
        char buff[256];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buff, 256, fmt, args);
        va_end(args);

        if(SERIAL_DEBUG)
            SERIAL_DEBUG.print(buff);
        if (Wifi_Helper::IsEnable() && Wifi_Helper::IsClientConnected())
            WIFI_DEBUG.print(buff);
        //Logger::Enqueue(buff, false);
    }

    void println(const char *fmt, ...)
    {
        if (!IsEnable())
            return;
        if (!fmt)
            fmt = "";
        char buff[256];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buff, 256, fmt, args);
        va_end(args);
        
        if(SERIAL_DEBUG)
            SERIAL_DEBUG.println(buff);
        if (Wifi_Helper::IsEnable() && Wifi_Helper::IsClientConnected())
            WIFI_DEBUG.println(buff);
        //Logger::Enqueue(buff, true);
    }

    void println()
    {
        if (!IsEnable())
            return;
            
        if(SERIAL_DEBUG)
            SERIAL_DEBUG.println();
        if (Wifi_Helper::IsEnable() && Wifi_Helper::IsClientConnected())
            WIFI_DEBUG.println();
        //Logger::Enqueue("", true);
    }
        
    void print(const String &str)
    {
        if (!IsEnable())
            return;
            
        if(SERIAL_DEBUG)
            SERIAL_DEBUG.print(str);
        if (Wifi_Helper::IsEnable() && Wifi_Helper::IsClientConnected())
            WIFI_DEBUG.print(str);
        //Logger::Enqueue(str.c_str(), false);
    }

    void println(const String &str)
    {
        if (!IsEnable())
            return;
            
        if(SERIAL_DEBUG)
            SERIAL_DEBUG.println(str);
        if (Wifi_Helper::IsEnable() && Wifi_Helper::IsClientConnected())
            WIFI_DEBUG.println(str);
        //Logger::Enqueue(str.c_str(), true);
    }

    void printError(const String &error)
    {
        println("!!!---------- ERROR ----------!!!");
        println(error);
        println("!!!---------- ERROR ----------!!!");
    }

    void printChrono(Chrono chrono)
    {
        if(Chrono::print)
        {
            int time = (int)(chrono.ElapsedTime()/chrono.LoopCount());
            int maxTime = (int)chrono.MaxTime();
            int minTime = (int)chrono.MinTime();
            uint stackSize = uxTaskGetStackHighWaterMark(nullptr);
            println("Chrono [%s] avg=%d µs, max=%d µs, min=%d µs  --  Stack Size %u", chrono.Name().c_str(), time, maxTime, minTime, stackSize);
        }
        //if(Chrono::teleplot)
        //    teleplot(name,time);
    }

    void teleplot(const String &varName, const String &text)
    {
        if (teleplotUDP.IsInitialized())
            teleplotUDP.update(varName.c_str(), text.c_str(),"", 0, "|t");
        else if (teleplotEnable == Enable::ENABLE_TRUE)
            println(">%s:%s|t", varName.c_str(), text.c_str());            
    }

    void teleplot(const String &varName, int var)
    {
        if (teleplotUDP.IsInitialized())
            teleplotUDP.update(varName.c_str(), var);
        else if (teleplotEnable == Enable::ENABLE_TRUE)
            println(">%s:%d", varName.c_str(), var);            
    }

    void teleplot(const String &varName, float var)
    {
        if (teleplotUDP.IsInitialized())
            teleplotUDP.update(varName.c_str(), var);
        else if (teleplotEnable == Enable::ENABLE_TRUE)
            println(">%s:%f", varName.c_str(), var);            
    }

    void teleplot(const String &varName, Point point)
    {
        if (teleplotUDP.IsInitialized())
            teleplotUDP.update2D(varName.c_str(), point.x, point.y);
        else if (teleplotEnable == Enable::ENABLE_TRUE)
            println(">%s:%i:%i|xy", varName.c_str(), point.x, point.y);
    }

    void teleplot(const String &varName, PointF point)
    {
        if (teleplotUDP.IsInitialized())
            teleplotUDP.update2D(varName.c_str(), point.x, point.y);
        else if (teleplotEnable == Enable::ENABLE_TRUE)
            println(">%s:%f:%f|xy", varName.c_str(), point.x, point.y);
    }

    void teleplot(const String &varName, Pose pose)
    {
        if (teleplotUDP.IsInitialized())
            teleplotUDP.update2D(varName.c_str(), pose.x, pose.y);
        else if (teleplotEnable == Enable::ENABLE_TRUE)
            println(">%s:%i:%i|xy", varName.c_str(), pose.x, pose.y);
    }

    void teleplot(const String &varName, PoseF pose)
    {
        if (teleplotUDP.IsInitialized())
            teleplotUDP.update2D(varName.c_str(), pose.x, pose.y);
        else if (teleplotEnable == Enable::ENABLE_TRUE)
            println(">%s:%f:%f|xy", varName.c_str(), pose.x, pose.y);
    }

    void teleplot(const String &varName, PolarPoint polarPoint)
    {
        if (teleplotUDP.IsInitialized())
            teleplotUDP.update2D(varName.c_str(), polarPoint.x, polarPoint.y);
        else if (teleplotEnable == Enable::ENABLE_TRUE)
            println(">%s:%f:%f|xy", varName.c_str(), polarPoint.x, polarPoint.y);
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
