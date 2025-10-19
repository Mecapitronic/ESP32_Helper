/*
    Start the serial communication at default baud rate speed : 921600
    SERIAL_DEBUG is the Serial port used for communication with the PC
    You can redefine it to change the Serial port used
*/

#include "ESP32_Helper.h"
using namespace Printer;

int32_t cpt;
void setup(void)
{
    ESP32_Helper::Initialisation();    

    cpt = Preferences_Helper::LoadFromPreference("cpt", -1);
    println("Load cpt = ", cpt);

    delay(3000);

    if (!FileSystem_Helper::Initialisation())
    {
        ESP.restart();
        return;
    }

    FileSystem_Helper::ListFiles();
    FileSystem_Helper::CreateFile("hello.txt");
    FileSystem_Helper::ListFiles();
    println(FileSystem_Helper::ReadFile("hello.txt"));
    FileSystem_Helper::WriteFile("hello.txt", "Hello ");
    println(FileSystem_Helper::ReadFile("hello.txt"));
    FileSystem_Helper::AppendFile("hello.txt", String(cpt));
    println(FileSystem_Helper::ReadFile("hello.txt"));
    FileSystem_Helper::AppendFile("hello.txt", " World!");
    println(FileSystem_Helper::ReadFile("hello.txt"));
    FileSystem_Helper::AppendFile("hello.txt", "\r\n");
    println(FileSystem_Helper::ReadFile("hello.txt"));
    FileSystem_Helper::RenameFile("hello.txt", "world.txt");
    println(FileSystem_Helper::ReadFile("world.txt"));
    FileSystem_Helper::DeleteFile("world.txt");
    println(FileSystem_Helper::ReadFile("world.txt"));
    FileSystem_Helper::ListFiles();

    String content = "{\r\n\t\"object\":\r\n\t{\r\n\t\t\"key1\": \"value1\",\r\n\t\t\"key2\": \"value2\"\r\n\t}\r\n}";
    FileSystem_Helper::WriteFile("hello.json", content, true);
    FileSystem_Helper::ListFiles();    
    println(FileSystem_Helper::ReadFile("hello.json"));
    FileSystem_Helper::DeleteFile("hello.json");
    println(FileSystem_Helper::ReadFile("hello.json"));
    FileSystem_Helper::ListFiles();
    
    println(FileSystem_Helper::ReadFile("cpt"));
}

void loop(void)
{
    delay(1000);
    FileSystem_Helper::AppendFile("cpt", String(cpt++)+"\r\n",true);
}
