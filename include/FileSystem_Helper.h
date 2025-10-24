#ifndef FILE_SYSTEM_HELPER_H
#define FILE_SYSTEM_HELPER_H

#if defined(SIMULATOR)
#include "MockSPIFFS.h"
#define SPIFFS MockSPIFFS
#define File MockFile
#else
#include "SPIFFS.h"
#endif
#include "ESP32_Helper.h"

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true

namespace FileSystem_Helper
{
    bool Initialisation();

    void FormatFS();

    void ListFiles(String filter = "");

    void CreateFile(String fileName);
    
    String ReadFile(String fileName);

    void WriteFile(String fileName, String message, bool createFile = false);

    void AppendFile(String fileName, String message, bool createFile = false);

    void RenameFile(String fileName1, String fileName2);

    void DeleteFile(String fileName);

    void TestFileIO(String fileName);

    void HandleCommand(Command cmdTmp);

    void PrintCommandHelp();
}
#endif