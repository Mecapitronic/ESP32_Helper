#include "ServoAX12.h"

using namespace Printer;

namespace ServoAX12
{
    namespace
    {
        bool _simulation = false;
        int8_t _rxPin;
        int8_t _txPin;
        int8_t _dirPin;
        BaudRate _baudRate;
        DxlProtocolVersion _dxlProtocolVersion;
        Dynamixel2Arduino dxl;

        std::unordered_map<uint8_t, ServoMotion, std::hash<uint8_t>> Servos;
    }

    void Initialisation(HardwareSerial &serial, int8_t rxPin, int8_t txPin,int8_t dirPin, BaudRate baudRate, DxlProtocolVersion dxlProtocolVersion)
    {
        _rxPin =rxPin;
        _txPin=txPin;
        _dirPin=dirPin;
        _baudRate = baudRate;
        _dxlProtocolVersion = dxlProtocolVersion;
        //_simulation = simulation;
        Serial1.setPins(_rxPin, _txPin);
        dxl = Dynamixel2Arduino(Serial1, _dirPin);
        // Set Port baudrate. This has to match with DYNAMIXEL baudrate.
        dxl.begin((unsigned long)_baudRate);
        // Set Port Protocol Version. This has to match with DYNAMIXEL protocol version.
        dxl.setPortProtocolVersion((float)_dxlProtocolVersion);

        Servos.clear();

        InitAllServo();
        
        //TaskThread(TaskUpdateServo, "TaskUpdateServo", 2000, 15, 1);
    }
    
    void TaskUpdateServo(void *pvParameters)
    {
        println("Start TaskUpdateServo");
        while (true)
        {
            try
            {
                // take some time to update the servo
                ServoAX12::UpdateAllServo();
            }
            catch (const std::exception &e)
            {
                printError(e.what());
            }
            vTaskDelay(10);
        }
    }

    void AddServo(uint8_t id, String name, uint16_t positionMin, uint16_t positionMax)
    {
        Servos[id] = ServoMotion(id, name, positionMin, positionMax);
        InitServo(Servos.at(id));
    }
    
    void InitAllServo()
    {
        for (auto &[id, servo] : Servos)
        {
            InitServo(servo);
        }   
    }

    void InitServo(ServoMotion &servo)
    {
        print("Init Servo ID : %i name : %s", servo.id, servo.name.c_str());
        if (_simulation)
        {
            servo.position = servo.command_position = (float)servo.positionMin;
            println("Servo %s %d position: %f", servo.name, servo.id, servo.position);
            return;
        }
        int retry = 10;
        bool pingOK = false;
        while (!pingOK && retry>=0)
        {
            if(dxl.ping(servo.id))
            {
                pingOK = true;
                break;
            }
            retry--;
            print(".");
            vTaskDelay(10);
        }
        if(pingOK)
        {
            PrintDxlInfo(servo.id);

            servo.ledState = true;
            dxl.ledOn(servo.id);

            // Turn off torque when configuring items in EEPROM area
            dxl.torqueOff(servo.id);
            dxl.setOperatingMode(servo.id, OP_POSITION);
            dxl.torqueOn(servo.id);

            servo.position = servo.command_position = dxl.getPresentPosition(servo.id, UNIT_DEGREE);
            
            println(" position: %f", servo.position);
            //SetServoPosition(servo.id, servo.position);
        }
        else
        {
            println(" NOT connected !");
        }
    }

    void StopAllServo()
    {
        println("Stop All Servo");
        for (auto &[id, servo] : Servos)
        {
            StopServo(servo);
        }
    }

    void StopServo(ServoMotion &servo)
    {
        if (!_simulation)
        {
            dxl.torqueOff(servo.id);
            dxl.ledOff(servo.id);
        }
    }

    void StartAllServo()
    {
        println("Start All Servo");
        for (auto &[id, servo] : Servos)
        {
            StartServo(servo);
        }
    }

    void StartServo(ServoMotion &servo)
    {
        if (!_simulation)
            dxl.torqueOn(servo.id);
    }

    void UpdateAllServo()
    {
        // 1 ms / servo
        for (auto &[id, servo] : Servos)
        {
            UpdateServo(servo);
        }
    }

    void UpdateServo(ServoMotion &servo)
    {
        if (_simulation)
        {
            servo.position = servo.position + (servo.command_position - servo.position) / 2;
        }
        else
            servo.position = dxl.getPresentPosition(servo.id, UNIT_DEGREE);

        if (servo.position >= servo.command_position + 5
            || servo.position <= servo.command_position - 5)
        {
            if (!servo.ledState)
            {
                servo.ledState = true;
                if (!_simulation)
                    dxl.ledOn(servo.id);
            }
            servo.IsMoving = true;
        }
        else
        {
            if (servo.ledState)
            {
                servo.ledState = false;
                if (!_simulation)
                    dxl.ledOff(servo.id);
            }
            servo.IsMoving = false;
        }
    }

    bool AreAllServoMoving()
    {
        for (const auto &[id, servo] : Servos)
        {
            if (servo.IsMoving)
            {
                return true;
            }
        }
        return false;
    }

    bool IsServoMoving(uint8_t id)
    {
        return Servos.at(id).IsMoving;
    }

    void SetServoPosition(uint8_t id, float position)
    {
        if (position < (float)Servos.at(id).positionMin || position > (float)Servos.at(id).positionMax)
        {
            println("Position out of range for Servo ID : %i", id);
            println("Position : %f", position);
            println("Min : %f", (float)Servos.at(id).positionMin);
            println("Max : %f", (float)Servos.at(id).positionMax);
            return;
        }
        Servos.at(id).command_position = position;
        Servos.at(id).IsMoving = true;
        if(!_simulation)
            dxl.setGoalPosition(id, Servos.at(id).command_position, UNIT_DEGREE);
    }

    void HandleCommand(Command cmd)
    {
        if (cmd.cmd == "AX12Scan")
        {
            //AX12Scan:1:1000000
            if (cmd.size == 2)
            {
                Scan((DxlProtocolVersion)cmd.data[0],(BaudRate)cmd.data[1]);
            }
            else
            {
                Scan();
            }
        }
        else if (cmd.cmd == "AX12PrintInfo")
        {
            if (cmd.size == 1)
            {
                PrintDxlInfo(cmd.data[0]);
            }
            else
                PrintDxlInfo();
        }
        else if (cmd.cmd == "AX12AddServo")
        {
            //AX12AddServo:1:Left:0:300
            if (cmd.size == 3 && cmd.dataStr1 != "")
            {
                AddServo(cmd.data[0], cmd.dataStr1, cmd.data[1], cmd.data[2]);
            }
        }
        else if (cmd.cmd == "AX12Pos")
        {
            
            if (cmd.size == 2)
            {
                // AX12Pos:1:100
                uint8_t id = cmd.data[0];
                print("AX12 Servo id: %i ", id);
                if (Servos.count(id) == 0)
                {
                    println("is not initialized");
                }
                else
                {
                    UpdateServo(Servos.at(id));
                    println("Set Position : %i", cmd.data[1]);
                    SetServoPosition(id, (float)cmd.data[1]);
                }
            }
            else
            {
                UpdateAllServo();
                // TeleplotPosition();
                PrintPosition();
            }
        }
        else if (cmd.cmd == "AX12Stop")
        {
            println("AX12Stop");
            StopAllServo();
        }
        else if (cmd.cmd == "AX12Start")
        {
            println("AX12Start");
            StartAllServo();
        }
        else if (cmd.cmd == "AX12Help")
        {
            PrintCommandHelp();
        }
        else
        {
            println("Unknown command : ", cmd.cmd);
        }
    }

    const void PrintCommandHelp()
    {
        Printer::println("AX12 Command Help :");
        Printer::println(" > AX12Scan");
        Printer::println("      Scan all Dynamixel on all protocols and baudrates");
        Printer::println(" > AX12PrintInfo:[id]");
        Printer::println("      Print info for all servos or for the given id");
        Printer::println(" > AX12AddServo:[id]:[name]:[min]:[max]");
        Printer::println("      Add a servo with the given id, name, min and max position");
        Printer::println(" > AX12Pos:[id]:[position]");
        Printer::println("      Set servo [id] to [position] (in degrees)");
        Printer::println("      If no argument, print all currents positions");
        Printer::println(" > AX12Stop");
        Printer::println("      Stop all servos (torque off)");
        Printer::println(" > AX12Help");
        Printer::println("      Print this help");
        Printer::println();
    }

    int16_t Scan()
    {
        int16_t found_dynamixel = 0;
        for (auto &&proto : dxlProtocol)
        {
            for (auto &&baud : dxlBaud)
            {
                found_dynamixel += Scan(proto, baud);
                vTaskDelay(1);
            }
        }
        println("Total : %i Dynamixel(s) found", found_dynamixel);
        return found_dynamixel;
    }

    int16_t Scan(DxlProtocolVersion _protocol, BaudRate _dxlBaud)
    {
        //Save original protocol and version
        float version = dxl.getPortProtocolVersion();
        unsigned long baud = dxl.getPortBaud();
        int16_t found_dynamixel = 0;
        // Set Port Protocol Version. This has to match with DYNAMIXEL protocol version.
        dxl.setPortProtocolVersion((float)_protocol);
        print("Scan Protocol %f - ", (float)_protocol);

        // Set Port baudrate.
        dxl.begin((int)_dxlBaud);
        println("Scan Baudrate %i", (int)_dxlBaud);
        //for (int id = 0; id < DXL_BROADCAST_ID; id++)
        for (int id = 11; id < 13; id++)
        {
            // iterate until all ID in each baudrate is scanned.
            if (dxl.ping(id))
            {
                println("ID : %i, Model Number: %i",id, dxl.getModelNumber(id));
                found_dynamixel++;                
            }
            vTaskDelay(1);
        }
        println("Found %i Dynamixel(s)", found_dynamixel);

        //Put back original protocol and version
        dxl.setPortProtocolVersion(version);
        dxl.begin(baud);

        return found_dynamixel;
    }

    void PrintDxlInfo(uint8_t id)
    {
        if(id != DXL_BROADCAST_ID)
        {
            if (dxl.ping(id))
            {
                println("ID : %i, Name: %s, Model Number: %i, position: %f, command: %f, isMoving: %i",
                     id, Servos.at(id).name, dxl.getModelNumber(id), Servos.at(id).position, Servos.at(id).command_position, Servos.at(id).IsMoving);
            }
            else
            {
                println("Dynamixel ID : %i not found", id);
            }
        }
        else
        {
            for (const auto &[_id, servo] : Servos)
            {
                println("ID : %i, Name: %s, Model Number: %i, position: %f, command: %f, isMoving: %i",
                     _id, servo.name, dxl.getModelNumber(_id), servo.position, servo.command_position, servo.IsMoving);
            }
        }
    }

    void TeleplotPosition()
    {
        for (auto &[id, servo] : Servos)
        {
            teleplot("Servo " + servo.name + " " + String(servo.id), servo.position);
        }
    }

    void PrintPosition()
    {
        for (auto &[id, servo] : Servos)
        {
            println("Servo %s %d : %f", servo.name, servo.id, servo.position);
        }
    }
} // namespace ServoAX12