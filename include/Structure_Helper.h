/**
 * @file Structure_Helper.h
 * @author Mecapitronic (mecapitronic@gmail.com)
 * @brief All data structures used across program
 * @date 2023-07-25
 */
#ifndef STRUCTURE_HELPER_H
#define STRUCTURE_HELPER_H

#include <Arduino.h>
#include <queue>

using namespace std;
// #define private public // troll ...

enum class Level
{
    LEVEL_VERBOSE,
    LEVEL_INFO,
    LEVEL_WARN,
    LEVEL_ERROR,
    LEVEL_NONE
};

enum class Enable
{
    ENABLE_FALSE,
    ENABLE_TRUE,
    ENABLE_NONE
};

enum class Team 
{
    TEAM_BLUE,
    TEAM_YELLOW
};

enum class Mode 
{
    MODE_MATCH,
    MODE_TEST
};


enum class BaudRate
{
    BAUD_RATE_300 = 300,
    BAUD_RATE_600 = 600,
    BAUD_RATE_1200 = 1200,
    BAUD_RATE_2400 = 2400,
    BAUD_RATE_4800 = 4800,
    BAUD_RATE_9600 = 9600,
    BAUD_RATE_19200 = 19200,
    BAUD_RATE_38400 = 38400,
    BAUD_RATE_57600 = 57600,
    BAUD_RATE_74880 = 74880,
    BAUD_RATE_115200 = 115200,
    BAUD_RATE_230400 = 230400,
    BAUD_RATE_256000 = 256000,
    BAUD_RATE_460800 = 460800,
    BAUD_RATE_921600 = 921600,
    BAUD_RATE_1843200 = 1843200,
    BAUD_RATE_3686400 = 3686400
};

/**
 * @brief Represents a Cartesian point with x and y as coordinates
 * @param x int16_t
 * @param y int16_t
 */
struct Point
{
    int16_t x = 0;
    int16_t y = 0;
    /**
     * @brief Construct a new Point object
     *
     */
    Point() = default;
    /**
     * @brief Construct a new Point object
     * @param _x int16_t
     * @param _y int16_t
     */
    Point(int16_t _x, int16_t _y) : x(_x), y(_y) {}
};

/**
 * @brief Represents a Cartesian point with x and y as coordinates
 * @param x float
 * @param y float
 */
struct PointF
{
    float x = 0;
    float y = 0;
    /**
     * @brief Construct a new Point F object
     *
     */
    PointF() = default;
    /**
     * @brief Construct a new Point F object
     *
     * @param _x float
     * @param _y float
     */
    PointF(float _x, float _y) : x(_x), y(_y) {}
};

/**
 * @brief Represents a 2D Int Cartesian pose of object with position x, y and pointing direction h (heading)
 * @param x int16_t
 * @param y int16_t
 * @param h int32_t
 */
struct Pose
{
    int16_t x = 0;
    int16_t y = 0;
    int32_t h = 0;
    /**
     * @brief Construct a new Pose object
     */
    Pose() = default;
    /**
     * @brief Construct a new Pose object
     *
     * @param _x int16_t
     * @param _y int16_t
     * @param _h int32_t
     */
    Pose(int16_t _x, int16_t _y, int32_t _h) : x(_x), y(_y), h(_h) {}
};

/**
 * @brief Represents a 2D Float Cartesian pose of object with position x, y and pointing direction h (heading)
 * @param x float
 * @param y float
 * @param h float
 */
struct PoseF
{
    float x = 0;
    float y = 0;
    float h = 0;
    /**
     * @brief Construct a new PoseF object
     */
    PoseF() = default;
    /**
     * @brief Construct a new PoseF object
     *
     * @param _x float
     * @param _y float
     * @param _h float
     */
    PoseF(float _x, float _y, float _h) : x(_x), y(_y), h(_h) {}
};

/**
 * @brief Represents a 3D Cartesian point with x, y and z as coordinates
 * @param x int16_t
 * @param y int16_t
 * @param z int16_t
 */
struct Point3D
{
    int16_t x = 0;
    int16_t y = 0;
    int16_t z = 0;
    /**
     * @brief Construct a new Point3D object
     */
    Point3D() = default;
    /**
     * @brief Construct a new Point3D object
     *
     * @param _x int16_t
     * @param _y int16_t
     * @param _z int16_t
     */
    Point3D(int16_t _x, int16_t _y, int16_t _z) : x(_x), y(_y), z(_z) {}
};

/**
 * @brief Represents a 3D Cartesian point with x, y and z as coordinates
 * @param x float
 * @param y float
 * @param z float
 */
struct PointF3D
{
    float x = 0;
    float y = 0;
    float z = 0;
    /**
     * @brief Construct a new Point3D object
     */
    PointF3D() = default;
    /**
     * @brief Construct a new Point3D object
     *
     * @param _x float
     * @param _y float
     * @param _z float
     */
    PointF3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

/**
 * @brief Represents a 4D point with x, y and z as Cartesian coordinates, and w as 4th dimension
 * @param x int16_t
 * @param y int16_t
 * @param z int16_t
 * @param w int16_t
 */
struct Point4D
{
    int16_t x = 0;
    int16_t y = 0;
    int16_t z = 0;
    int16_t w = 0;
    /**
     * @brief Construct a new Point4D object
     */
    Point4D() = default;
    /**
     * @brief Construct a new Point4D object
     *
     * @param _x int16_t
     * @param _y int16_t
     * @param _z int16_t
     * @param _w int16_t
     */
    Point4D(int16_t _x, int16_t _y, int16_t _z, int16_t _w) : x(_x), y(_y), z(_z), w(_w) {}
};

/**
 * @brief Represents a 4D point with x, y and z as Cartesian coordinates, and w as 4th dimension
 * @param x float
 * @param y float
 * @param z float
 * @param w float
 */
struct PointF4D
{
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 0;
    /**
     * @brief Construct a new Point4D object
     */
    PointF4D() = default;
    /**
     * @brief Construct a new Point4D object
     *
     * @param _x float
     * @param _y float
     * @param _z float
     * @param _w float
     */
    PointF4D(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};

/**
 * @brief Represents a Polar point
 *  with angle, distance, Cartesian coordinates, and confidence
 * @param angle float Degree
 * @param distance int16_t mm
 * @param confidence uint16_t
 * @param x float
 * @param y float
 *
 */
struct PolarPoint
{
    float angle = 0;
    int16_t distance = 0;
    uint16_t confidence = 0;
    float x = 0;
    float y = 0;
    /**
     * @brief Construct a new Polar Point object
     */
    PolarPoint() = default;
    /**
     *
     * @brief Construct a new Polar Point object
     *
     * @param _angle
     * @param _distance
     * @param _confidence
     */
    PolarPoint(float _angle, int16_t _distance, uint16_t _confidence) : angle(_angle), distance(_distance), confidence(_confidence){}
    /**
     * @brief Construct a new Polar Point object
     *
     * @param _x
     * @param _y
     */
    PolarPoint(float _x, float _y) : x(_x), y(_y) {}
    /**
     * @brief Construct a new Polar Point object
     * @param _angle
     * @param _distance
     * @param _confidence
     * @param _x
     * @param _y
     */
    PolarPoint(float _angle, int16_t _distance, uint16_t _confidence, float _x, float _y) : angle(_angle), distance(_distance), confidence(_confidence), x(_x), y(_y) {}
};

struct PointTracker
{
    // point coordinates being tracked
    PolarPoint point;
    // timestamp of the last time the tracker has been updated
    int64_t lastUpdateTime;
    // express the confidence we have in the obstacle position based on how much time we've seen it.
    // It is increased it at every detection up to 4 times.
    // This is used as a trigger to send the obstacle to the robot (only when we are confident enough)
    int32_t confidence;
    // avoid to send the same points twice
    bool hasBeenSent = false;
};

struct Command
{
    static const int8_t length = 6;
    static const int8_t sizeStr = 14;
    String cmd = "";
    int8_t size = 0;
    std::array<int32_t, length> data;
    String dataStr = ""; //sizeof(String) = 16 BUT 1 is for string length and 1 for \0 so 14 in reality

    Command()
    {
        data.fill(0);
    }
};

template <typename T>
void pop_front(std::vector<T>& vec)
{
    if (vec.empty())
        return;
    vec.erase(vec.begin());
};

// converts character array
// to string and returns it
/*String convertToString(char* a, int32_t size)
{
    String s = "";
    for (int32_t i = 0; i < size; i++)
    {
        s = s + a[i];
    }
    return s;
};
*/

#endif
