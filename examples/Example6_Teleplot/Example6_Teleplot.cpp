/*
    Start the serial communication at default baud rate speed : 921600
    SERIAL_DEBUG is the Serial port used for communication with the PC
    You can redefine it to change the Serial port used
*/

#ifndef WITH_WIFI
#define WITH_WIFI
#endif
#ifndef OTA
#define OTA
#endif

#include "ESP32_Helper.h"
using namespace Printer;

void setup(void)
{
    ESP32_Helper::Initialisation();
    Printer::teleplotUDPEnable = Enable::ENABLE_TRUE;
}

static int32_t cpt = 0;
Point point = Point(0, 0);
PoseF pose = PoseF(0.0f, 0.0f, 0.0f);
int64_t timeSinceBoot = 0;

void loop(void)
{
    // This will be send over Wifi if enable and also on Serial
    println("Hello World %i times !", cpt++);
    point.x = cpt + cpt;
    point.y = cpt * cpt;
    pose.x = cpt / 3.0f;
    pose.y = cpt * 1.2f;
    pose.h = pose.x + pose.y;
    delay(1000);
    timeSinceBoot = esp_timer_get_time();

    teleplot("time", timeSinceBoot);
    teleplot("cpt", cpt);
    teleplot("point", point);
    teleplot("pose", pose);
}
