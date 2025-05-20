#include "Printer.h"

namespace Printer
{
    Teleplot teleplotUDP;
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
    /*
        void print(String prefix, String suffix, Level level)
        {
            if (!IsPrintable(level))
                return;
            print(prefix);
            print(suffix);
        }

        void println(String prefix, String suffix, Level level)
        {
            if (!IsPrintable(level))
                return;
            print(prefix, suffix, level);
            println();
        }
    */
   
    void println(Level level)
    {
        if (!IsPrintable(level))
            return;
        if(Wifi_Helper::IsEnable())
            WIFI_DEBUG.println();
        //else
        SERIAL_DEBUG.println();
    }

    void print(int32_t data, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(String(data));
    }

    void println(int32_t data, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(data);
        println();
    }

    void print(String prefix, int32_t data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix);
        print(data);
        print(suffix);
    }

    void println(String prefix, int32_t data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix, data, suffix, level);
        println();
    }

    void print(uint32_t data, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(String(data));
    }

    void println(uint32_t data, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(data);
    }

    void print(String prefix, uint32_t data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix);
        print(data);
        print(suffix);
    }

    void println(String prefix, uint32_t data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix, data, suffix, level);
        println();
    }

    void print(char data, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(String(data));
    }

    void println(char data, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(data);
        println();
    }

    void print(String prefix, char data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix);
        print(data);
        print(suffix);
    }

    void println(String prefix, char data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix, data, suffix, level);
        println();
    }

    void print(float data, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(String(data));
    }

    void println(float data, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(data);
        println();
    }

    void print(String prefix, float data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix);
        print(data);
        print(suffix);
    }

    void println(String prefix, float data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix, data, suffix, level);
        println();
    }

    void print(unsigned long data, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(String(data));
    }

    void println(unsigned long data, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(data);
        println();
    }

    void print(String prefix, unsigned long data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix);
        print(data);
        print(suffix);
    }

    void println(String prefix, unsigned long data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix, data, suffix, level);
        println();
    }

    void print(long data, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(String(data));
    }

    void println(long data, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(data);
        println();
    }

    void print(String prefix, long data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix);
        print(data);
        print(suffix);
    }

    void println(String prefix, long data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix, data, suffix, level);
        println();
    }

    void print(String data, Level level)
    {
        if (!IsPrintable(level))
            return;            
        if(Wifi_Helper::IsEnable())
            WIFI_DEBUG.print(data);
        //else
        SERIAL_DEBUG.print(data);
    }

    void println(String data, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(data);
        println();
    }

    void print(String prefix, String data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix);
        print(data);
        print(suffix);
    }

    void println(String prefix, String data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix, data, suffix, level);
        println();
    }

    void print(String prefix, Point data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix);
        print("x: ");
        print(data.x);
        print(" y: ");
        print(data.y);
        print(suffix);
    }

    void println(String prefix, Point data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix, data, suffix, level);
        println();
    }

    void print(String prefix, Point3D data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix);
        print("x: ");
        print(data.x);
        print(" y: ");
        print(data.y);
        print(" z: ");
        print(data.z);
        print(suffix);
    }

    void println(String prefix, Point3D data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix, data, suffix, level);
        println();
    }

    void print(String prefix, Point4D data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix);
        print("x: ");
        print(data.x);
        print(" y: ");
        print(data.y);
        print(" z: ");
        print(data.z);
        print(" w: ");
        print(data.w);
        print(suffix);
    }

    void println(String prefix, Point4D data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix, data, suffix, level);
        println();
    }

    void print(String prefix, PolarPoint data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix);
        print("A: ");
        print((int)(data.angle / 100));
        print(" D: ");
        print(data.distance);
        print(" C: ");
        print(data.confidence);
        print(" X: ");
        print(data.x);
        print(" Y: ");
        print(data.y);
        print(suffix);
    }

    void println(String prefix, PoseF data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix, data, suffix, level);
        println();
    }

    void print(String prefix, PoseF data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix);
        print(" X: ");
        print(data.x);
        print(" Y: ");
        print(data.y);
        print(" H: ");
        print(data.h / 100);
        print(suffix);
    }

    void println(String prefix, PolarPoint data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix, data, suffix, level);
        println();
    }

    void print(String prefix, Command data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix);
        print(" Cmd=" + data.cmd);
        if (data.size > 0)
        {
            print(" Size=", data.size);
            print(" Data=", data.data[0]);
            for (int8_t size_data = 1; size_data < data.size; size_data++)
            {
                print(",", data.data[size_data]);
            }
        }
        if (data.dataStr != "")
        {
            print(" DataStr=", String(data.dataStr));
        }
    }

    void println(String prefix, Command data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix, data, suffix, level);
        println();
    }

    // bool needs to be the last because it overrides all functions
    void print(String prefix, bool data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix);
        if (data)
            print("true");
        else
            print("false");
        print(suffix);
    }

    void println(String prefix, bool data, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix, data, suffix, level);
        println();
    }

    void printArray(String prefix, int32_t array[], size_t size, char separator, String suffix, Level level)
    {
        if (!IsPrintable(level))
            return;
        print(prefix);
        if (size > 0)
        {
            for (size_t i = 0; i < size - 1; i++)
            {
                print(array[i]);
                print(separator);
            }
            print(array[size - 1]);
        }
        println(suffix);
    }

    void teleplot(String varName, float var, Level level)
    {
        if (!IsPrintable(level))
            return;
        String data = ">" + varName + ":" + var;
        if(teleplotUDP.IsInitialized())
            teleplotUDP.update(varName.c_str(), var);
        else
            println(data);
    }
    
    void teleplot(String varName, Point point, Level level)
    {
        if (!IsPrintable(level))
            return;
        String data = ">" + varName + ":" + point.x + ":" + point.y + "|xy";
        if(teleplotUDP.IsInitialized())
            teleplotUDP.update2D(varName.c_str(), point.x, point.y);
        else
            println(data);
    }

    void teleplot(String varName, PointF point, Level level)
    {
        if (!IsPrintable(level))
            return;
        String data = ">" + varName + ":" + point.x + ":" + point.y + "|xy";
        if(teleplotUDP.IsInitialized())
            teleplotUDP.update2D(varName.c_str(), point.x, point.y);
        else
            println(data);
    }

    void teleplot(String varName, Pose point, Level level)
    {
        if (!IsPrintable(level))
            return;
        String data = ">" + varName + ":" + point.x + ":" + point.y + "|xy";
        if(teleplotUDP.IsInitialized())
            teleplotUDP.update2D(varName.c_str(), point.x, point.y);
        else
            println(data);
    }

    void teleplot(String varName, PoseF point, Level level)
    {
        if (!IsPrintable(level))
            return;
        String data = ">" + varName + ":" + point.x + ":" + point.y + "|xy";
        if(teleplotUDP.IsInitialized())
            teleplotUDP.update2D(varName.c_str(), point.x, point.y);
        else
            println(data);
    }

/*
    void teleplot(String varName, Point points[], uint16_t size, Level level)
    {
        if (!IsPrintable(level))
            return;
        String data = ">" + varName + ":";
        for (uint16_t i = 0; i < size; i++)
        {
            data += String() + (int)(points[i].x) + ":" + (int)(points[i].y) + ";";
        }
        data += "|xy";
        println(data);
    }
*/

    void teleplot(String varName, PolarPoint polarPoint, Level level)
    {
        if (!IsPrintable(level))
            return;
        String data = ">" + varName + ":" + (int)polarPoint.x + ":" + (int)polarPoint.y + "|xy";
        if (teleplotUDP.IsInitialized())
            teleplotUDP.update2D(varName.c_str(), polarPoint.x, polarPoint.y);
        else
            println(data);
    }

/*
    void teleplot(String varName, PolarPoint polarPoint, int32_t timeStamp, Level level)
    {
        if (!IsPrintable(level))
            return;
        String data = ">" + varName + ":" + polarPoint.x + ":" + polarPoint.y + ":" + timeStamp + "|xy";
        println(data);
    }
*/
/*
    void teleplot(String varName, PolarPoint polarPoints[], uint16_t size, Level level)
    {
        if (!IsPrintable(level))
            return;
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
    void teleplot(String varName, vector<PolarPoint> vec, Level level)
    {
        if (!IsPrintable(level))
            return;
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
    void teleplot(String varName, Point4D point, Level level)
    {
        if (!IsPrintable(level))
            return;
        String data = ">" + varName + ":" + (int)point.x + ":" + (int)point.y + "|xy";
        println(data);
    }
*/
/*
    void plot3D(Point3D p, String varName)
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
