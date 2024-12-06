/**
 * @file Printer.h
 * @author Mecapitronic (mecapitronic@gmail.com)
 * @brief Used to print data over predefined serial for debug purpose
 * @date 2023-07-25
 */
#ifndef PRINTER_H
#define PRINTER_H

#include "ESP32_Helper.h"

#define PRINT_VAR_NAME(p) print(#p)
#define printVar(p) print(#p); println(" : ",p)

#define ENUM_PRINT(p)             \
    case (p):                     \
        SERIAL_DEBUG.println(#p); \
        break;

namespace Printer
{
    void SetLevel(Level level);
    void EnablePrinter(Enable enable);

    void Initialisation();
    void Update(void *pvParameters);
    void HandleCommand(Command cmdTmp);
    void PrintCommandHelp();

    bool IsPrintable(Level level = LEVEL_VERBOSE);

    void println(Level level = LEVEL_VERBOSE);

    //void print(String prefix, String suffix = "", Level level = LEVEL_VERBOSE);
    //void println(String prefix, String suffix = "", Level level = LEVEL_VERBOSE);

    void print(int data, Level level = LEVEL_VERBOSE);
    void println(int data, Level level = LEVEL_VERBOSE);
    void print(String prefix, int data, String suffix = "", Level level = LEVEL_VERBOSE);
    void println(String prefix, int data, String suffix = "", Level level = LEVEL_VERBOSE);

    void print(uint data, Level level = LEVEL_VERBOSE);
    void println(uint data, Level level = LEVEL_VERBOSE);
    void print(String prefix, uint data, String suffix = "", Level level = LEVEL_VERBOSE);
    void println(String prefix, uint data, String suffix = "", Level level = LEVEL_VERBOSE);

    void print(char data, Level level = LEVEL_VERBOSE);
    void println(char data, Level level = LEVEL_VERBOSE);
    void print(String prefix, char data, String suffix = "", Level level = LEVEL_VERBOSE);
    void println(String prefix, char data, String suffix = "", Level level = LEVEL_VERBOSE);

    void print(float data, Level level = LEVEL_VERBOSE);
    void println(float data, Level level = LEVEL_VERBOSE);
    void print(String prefix, float data, String suffix = "", Level level = LEVEL_VERBOSE);
    void println(String prefix, float data, String suffix = "", Level level = LEVEL_VERBOSE);

    void print(String data, Level level = LEVEL_VERBOSE);
    void println(String data, Level level = LEVEL_VERBOSE);
    void print(String prefix, String data, String suffix = "", Level level = LEVEL_VERBOSE);
    void println(String prefix, String data, String suffix = "", Level level = LEVEL_VERBOSE);

    void print(String prefix, Point data, String suffix = "", Level level = LEVEL_VERBOSE);
    void println(String prefix, Point data, String suffix = "", Level level = LEVEL_VERBOSE);

    void print(String prefix, Point3D data, String suffix = "", Level level = LEVEL_VERBOSE);
    void println(String prefix, Point3D data, String suffix = "", Level level = LEVEL_VERBOSE);

    void print(String prefix, Point4D data, String suffix = "", Level level = LEVEL_VERBOSE);
    void println(String prefix, Point4D data, String suffix = "", Level level = LEVEL_VERBOSE);

    void print(String prefix, PolarPoint data, String suffix = "", Level level = LEVEL_VERBOSE);
    void println(String prefix, PolarPoint data, String suffix = "", Level level = LEVEL_VERBOSE);
    
    void print(String prefix, Command data, String suffix = "", Level level = LEVEL_VERBOSE);
    void println(String prefix, Command data, String suffix = "", Level level = LEVEL_VERBOSE);

    // bool needs to be the last because it overrides all functions
    void print(String prefix, bool data, String suffix = "", Level level = LEVEL_VERBOSE);
    void println(String prefix, bool data, String suffix = "", Level level = LEVEL_VERBOSE);

    void printArray(String prefix, int array[], size_t size, char separator = ',', String suffix = "",
                    Level level = LEVEL_VERBOSE);

    /**
     * Send data to be viewed on teleplot, eg: trace x and y in a graph
     */
    void teleplot(String varName, float var, Level level = LEVEL_VERBOSE);
    void teleplot(String varName, Point point, Level level = LEVEL_VERBOSE);
    void teleplot(String varName, PointF point, Level level = LEVEL_VERBOSE);
    void teleplot(String varName, Point2D point, Level level = LEVEL_VERBOSE);
    void teleplot(String varName, PointF2D point, Level level = LEVEL_VERBOSE);
    
    void teleplot(String varName, Point points[], uint16_t size, Level level = LEVEL_VERBOSE);

    void teleplot(String varName, PolarPoint polarPoint, Level level = LEVEL_VERBOSE);
    //void teleplot(String varName, PolarPoint polarPoint, int timeStamp, Level level = LEVEL_VERBOSE);

    void teleplot(String varName, PolarPoint polarPoints[], uint16_t size, Level level = LEVEL_VERBOSE);
    void teleplot(String varName, vector<PolarPoint> vec, Level level = LEVEL_VERBOSE);

    void teleplot(String varName, Point4D point, Level level = LEVEL_VERBOSE);

    /**
     * send cloud point data on serial for teleplot to trace 3D shape
     */
    void plot3D(String varName, Point3D p);
    void plot3Dpy(Point3D p);
}
#endif
