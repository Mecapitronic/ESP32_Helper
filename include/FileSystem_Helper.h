#ifndef FILE_SYSTEM_HELPER_H
#define FILE_SYSTEM_HELPER_H

#include "ESP32_Helper.h"
#include <vector>

#ifdef SIMULATOR
#include "MockSPIFFS.h"
#define SPIFFS MockSPIFFS
#define File MockFile
#else
#include "SPIFFS.h"
#endif

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true

namespace FileSystem_Helper
{
    namespace AppendASCII
    {
        extern Timeout timeout;
        extern int bufferSize;
        extern std::vector<char> buffer;
        extern String fileName;
        void Reset();
    }

    bool Initialisation();

    void FormatFS();

    void ListFiles(const String &filter = "");

    bool CreateFile(const String &fileName);
    
    String ReadFile(const String &fileName);

    bool WriteFile(const String &fileName, const String &message, bool createFile = false);

    bool AppendFile(const String &fileName, const std::vector<char> &message, bool createFile = false);
    bool AppendFile(const String &fileName, const String &message, bool createFile = false);

    bool RenameFile(const String &fileName1, const String &fileName2);

    bool DeleteFile(const String &fileName);

    void TestFileIO(const String &fileName);

    bool HandleCommand(Command cmdTmp);

    void PrintCommandHelp();
}
#endif