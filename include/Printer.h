/**
 * @file Printer.h
 * @author Mecapitronic (mecapitronic@gmail.com)
 * @brief Simplified printer for serial/wifi output
 * @date 2025-11-17
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
    
    // Basic print functions
    void print(const char *fmt, ...);
    void println(const char *fmt, ...);
    void println();
    void print(const String &str);
    void println(const String &str);

    // Utility functions
    void printError(const String &error);
    void printChrono(Chrono chrono);

    /**
     * Send data to be viewed on teleplot, eg: trace x and y in a graph
     */
    void teleplot(const String &varName, float var);
    void teleplot(const String &varName, Point point);
    void teleplot(const String &varName, PointF point);
    void teleplot(const String &varName, Pose point);
    void teleplot(const String &varName, PoseF point);
    
    //void teleplot(const String &varName, Point points[], uint16_t size);

    void teleplot(const String &varName, PolarPoint polarPoint);
    //void teleplot(const String &varName, PolarPoint polarPoint, int32_t timeStamp);

    //void teleplot(const String &varName, PolarPoint polarPoints[], uint16_t size);
    //void teleplot(const String &varName, vector<PolarPoint> vec);

    //void teleplot(const String &varName, Point4D point);

    /**
     * send cloud point data on serial for teleplot to trace 3D shape
     */
    //void plot3D(const String &varName, Point3D p);
    //void plot3Dpy(Point3D p);
}
#endif
