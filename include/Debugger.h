/**
 * Helper functions to print and debug via serial
 */

#ifndef DEBUGGER_H
#define DEBUGGER_H

#define VERBOSE Level::LEVEL_VERBOSE
#define INFO Level::LEVEL_INFO
#define WARN Level::LEVEL_WARN
#define ERROR Level::LEVEL_ERROR

#include "ESP32_Helper.h"

#define SERIAL_DEBUG Serial
#define SERIAL_DEBUG_SPEED 921600
#define READ_SERIAL_BUFFER_SIZE 64

// ESP32 Serial Bauds rates
// static const unsigned long default_rates[] = {300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 74880,
// 115200, 230400, 256000, 460800, 921600, 1843200, 3686400};

enum class Level
{
    LEVEL_VERBOSE = 0,
    LEVEL_INFO = 1,
    LEVEL_WARN = 2,
    LEVEL_ERROR = 3
};

class Debugger
{
   private:
    static const boolean enabled = true;
    static const Level debugLevel = INFO;

   public:
    /**
     * Print custom header on debugging serial
     */
    static void header();
    /**
     * Initialize debugging serial for PC communication
     */
    static void init();

    /**
     * Check for commands send on debugging serial plugged to a computer
     * List of commands is still empty now
     */
    static char* checkSerial();

    /**
     * Return the debugging level used: VERBOSE, INFO,WARN or ERROR
     */
    static Level level();

    static void log(String data, Level level = VERBOSE, boolean lineFeed = true);
    static void log(String prefix, int data, String suffix = "", Level level = VERBOSE, boolean lineFeed = true);
    static void log(String prefix, char data, String suffix = "", Level level = VERBOSE, boolean lineFeed = true);
    static void log(String prefix, float data, String suffix = "", Level level = VERBOSE, boolean lineFeed = true);

    static void log(String prefix, Point data, String suffix = "", Level level = VERBOSE, boolean lineFeed = true);
    static void log(String prefix, PolarPoint data, String suffix = "", Level level = VERBOSE, boolean lineFeed = true);

    // bool needs to be the last because it overrides all functions
    static void log(String prefix, bool data, String suffix = "", Level level = VERBOSE, boolean lineFeed = true);

    static void logArray(String prefix, int array[], size_t size, char separator = ',', String suffix = ")", Level level = VERBOSE);
    static void logArrayN(String prefix, int element, String interFix, int array[], size_t size, char separator = ',', String suffix = ")", Level level = VERBOSE);

    /**
     * send point data on serial for teleplot to trace x and y in a graph
     */
    static void plotPoint(Point p, String varName, Level level = INFO);

    /**
     * send point data on serial for teleplot to trace x and y as two separate graphs
     */
    static void plotPoint(Point p, Level level = INFO);

    /**
     * send cloud point data on serial for teleplot to trace 3D shape
     */
    static void plot3D(Point3D p, String varName);
    static void plot3Dpy(Point3D p);

    static bool waitForAvaiableSteps();
    static void addSteps(uint16_t steps);
    static void subSteps(uint16_t steps = 1);
};
#endif