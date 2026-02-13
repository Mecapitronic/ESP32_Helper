/*
    Start the serial communication at default baud rate speed : 921600
    SERIAL_DEBUG is the Serial port used for communication with the PC
    You can redefine it to change the Serial port used
*/

#include "ESP32_Helper.h"
using namespace Printer;

void setup(void)
{
    delay(2000);
    ESP32_Helper::Initialisation();
    int8_t rxPin = 26;
    int8_t txPin = 27;
    int8_t dirPin = 14;

    ServoAX12::Initialisation(Serial1, rxPin, txPin, dirPin);

    ServoAX12::Scan(ServoAX12::DxlProtocolVersion::PROTOCOL_1, BaudRate::BAUD_RATE_1000000);
    
    ServoAX12::AddServo(1, "Up", 0, 300);
    ServoAX12::AddServo(2, "Down", 0, 300);
    ServoAX12::AddServo(11, "Left", 0, 300);
    ServoAX12::AddServo(12, "Right", 0, 300);
}

int32_t cpt = 0;

void loop(void)
{
    ServoAX12::SetServoPosition(1, 10);
    while (ServoAX12::IsServoMoving(1))
    {
        ServoAX12::TeleplotPosition(1);
        delay(5);
    }
    delay(1000);
    ServoAX12::SetServoPosition(1, 250);
    while (ServoAX12::IsServoMoving(1))
    {
        ServoAX12::TeleplotPosition(1);
        delay(5);
    }
    delay(1000);
}
