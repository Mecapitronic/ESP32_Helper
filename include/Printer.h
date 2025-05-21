/**
 * @file Printer.h
 * @author Mecapitronic (mecapitronic@gmail.com)
 * @brief Used to print data over predefined serial for debug purpose
 * @date 2023-07-25
 */
#ifndef PRINTER_H
#define PRINTER_H

#include "ESP32_Helper.h"
#include "Teleplot.h"

#define PRINT_VAR_NAME(p) print(#p)
#define printVar(p) print(#p); println(" : ",p)

#define ENUM_PRINT(p) \
    case (p):         \
        println(#p);  \
        break;

namespace Printer
{
    extern Teleplot teleplotUDP;
    extern Enable teleplotUDPEnable;

    void SetLevel(Level level);
    void EnablePrinter(Enable enable);
    bool IsEnable();
    bool IsPrintable(Level level);

    void Initialisation(Enable enable = Enable::ENABLE_TRUE, Level lvl = Level::LEVEL_VERBOSE);
    void Update(void *pvParameters);
    void HandleCommand(Command cmdTmp);
    void PrintCommandHelp();

    Level PrintLevel();
    void PrintLevel(Level level);
    Enable PrintEnable();
    void PrintEnable(Enable enable);
    
    void println(Level level = Level::LEVEL_VERBOSE);

    //void print(String prefix, String suffix = "", Level level = Level::LEVEL_VERBOSE);
    //void println(String prefix, String suffix = "", Level level = Level::LEVEL_VERBOSE);

    void print(int32_t data, Level level = Level::LEVEL_VERBOSE);
    void println(int32_t data, Level level = Level::LEVEL_VERBOSE);
    void print(String prefix, int32_t data, String suffix = "", Level level = Level::LEVEL_VERBOSE);
    void println(String prefix, int32_t data, String suffix = "", Level level = Level::LEVEL_VERBOSE);

    void print(uint32_t data, Level level = Level::LEVEL_VERBOSE);
    void println(uint32_t data, Level level = Level::LEVEL_VERBOSE);
    void print(String prefix, uint32_t data, String suffix = "", Level level = Level::LEVEL_VERBOSE);
    void println(String prefix, uint32_t data, String suffix = "", Level level = Level::LEVEL_VERBOSE);

    void print(char data, Level level = Level::LEVEL_VERBOSE);
    void println(char data, Level level = Level::LEVEL_VERBOSE);
    void print(String prefix, char data, String suffix = "", Level level = Level::LEVEL_VERBOSE);
    void println(String prefix, char data, String suffix = "", Level level = Level::LEVEL_VERBOSE);

    void print(float data, Level level = Level::LEVEL_VERBOSE);
    void println(float data, Level level = Level::LEVEL_VERBOSE);
    void print(String prefix, float data, String suffix = "", Level level = Level::LEVEL_VERBOSE);
    void println(String prefix, float data, String suffix = "", Level level = Level::LEVEL_VERBOSE);

    void print(long data, Level level = Level::LEVEL_VERBOSE);
    void println(long data, Level level = Level::LEVEL_VERBOSE);
    void print(String prefix, long data, String suffix = "", Level level = Level::LEVEL_VERBOSE);
    void println(String prefix, long data, String suffix = "", Level level = Level::LEVEL_VERBOSE);

    void print(unsigned long data, Level level = Level::LEVEL_VERBOSE);
    void println(unsigned long data, Level level = Level::LEVEL_VERBOSE);
    void print(String prefix, unsigned long data, String suffix = "", Level level = Level::LEVEL_VERBOSE);
    void println(String prefix, unsigned long data, String suffix = "", Level level = Level::LEVEL_VERBOSE);

    void print(String data, Level level = Level::LEVEL_VERBOSE);
    void println(String data, Level level = Level::LEVEL_VERBOSE);
    void print(String prefix, String data, String suffix = "", Level level = Level::LEVEL_VERBOSE);
    void println(String prefix, String data, String suffix = "", Level level = Level::LEVEL_VERBOSE);

    void print(String prefix, Point data, String suffix = "", Level level = Level::LEVEL_VERBOSE);
    void println(String prefix, Point data, String suffix = "", Level level = Level::LEVEL_VERBOSE);

    void print(String prefix, Point3D data, String suffix = "", Level level = Level::LEVEL_VERBOSE);
    void println(String prefix, Point3D data, String suffix = "", Level level = Level::LEVEL_VERBOSE);

    void print(String prefix, Point4D data, String suffix = "", Level level = Level::LEVEL_VERBOSE);
    void println(String prefix, Point4D data, String suffix = "", Level level = Level::LEVEL_VERBOSE);

    void print(String prefix, PolarPoint data, String suffix = "", Level level = Level::LEVEL_VERBOSE);
    void println(String prefix, PolarPoint data, String suffix = "", Level level = Level::LEVEL_VERBOSE);

    void print(String prefix, PoseF data, String suffix = "", Level level = Level::LEVEL_VERBOSE);
    void println(String prefix, PoseF data, String suffix = "", Level level = Level::LEVEL_VERBOSE);

    void print(String prefix, Command data, String suffix = "", Level level = Level::LEVEL_VERBOSE);
    void println(String prefix, Command data, String suffix = "", Level level = Level::LEVEL_VERBOSE);

    // bool needs to be the last because it overrides all functions
    void print(String prefix, bool data, String suffix = "", Level level = Level::LEVEL_VERBOSE);
    void println(String prefix, bool data, String suffix = "", Level level = Level::LEVEL_VERBOSE);

    void printArray(String prefix, int32_t array[], size_t size, char separator = ',', String suffix = "",
                    Level level = Level::LEVEL_VERBOSE);

    /**
     * Send data to be viewed on teleplot, eg: trace x and y in a graph
     */
    void teleplot(String varName, float var, Level level = Level::LEVEL_VERBOSE);
    void teleplot(String varName, Point point, Level level = Level::LEVEL_VERBOSE);
    void teleplot(String varName, PointF point, Level level = Level::LEVEL_VERBOSE);
    void teleplot(String varName, Pose point, Level level = Level::LEVEL_VERBOSE);
    void teleplot(String varName, PoseF point, Level level = Level::LEVEL_VERBOSE);
    
    //void teleplot(String varName, Point points[], uint16_t size, Level level = Level::LEVEL_VERBOSE);

    void teleplot(String varName, PolarPoint polarPoint, Level level = Level::LEVEL_VERBOSE);
    //void teleplot(String varName, PolarPoint polarPoint, int32_t timeStamp, Level level = Level::LEVEL_VERBOSE);

    //void teleplot(String varName, PolarPoint polarPoints[], uint16_t size, Level level = Level::LEVEL_VERBOSE);
    //void teleplot(String varName, vector<PolarPoint> vec, Level level = Level::LEVEL_VERBOSE);

    //void teleplot(String varName, Point4D point, Level level = Level::LEVEL_VERBOSE);

    /**
     * send cloud point data on serial for teleplot to trace 3D shape
     */
    //void plot3D(String varName, Point3D p);
    //void plot3Dpy(Point3D p);
}
#endif
