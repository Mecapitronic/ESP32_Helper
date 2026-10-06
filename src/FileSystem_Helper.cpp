#include "FileSystem_Helper.h"

using namespace Printer;
using namespace std;

namespace FileSystem_Helper
{
    bool Initialisation()
    {
        if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
        {
            println("SPIFFS Mount Failed");
            return false;
        }
        println("SPIFFS Mounted Successfully");
        return true;
    }

    void FormatFS()
    {
        println("Formatting SPIFFS...");
        if (SPIFFS.format())
        {
            println("SPIFFS Formatted Successfully");
        }
        else
        {
            println("SPIFFS Format Failed");
        }
    }

    void ListFiles(const String &filter)
    {
        if (filter != "")
            println("Listing files with filter <%s>", filter.c_str());
        else
            println("Listing files");

        File root = SPIFFS.open("/");
        if (!root)
        {
            println("Failed to open root directory");
            return;
        }

        File file = root.openNextFile();
        println("  FILE \t SIZE");
        while (file)
        {
            if (filter == "" || (filter != "" && String(file.name()).indexOf(filter) != -1))
            {
                println("  %s \t %i", file.name(), (int)file.size());
            }
            file = root.openNextFile();
        }
        root.close();
    }

    bool CreateFile(const String &fileName)
    {
        println("Creating file " + fileName);
        
        if(SPIFFS.exists("/" + fileName))
        {
            println("File already exists");
            return false;
        }

        File file = SPIFFS.open("/" + fileName, FILE_WRITE);
        if (!file)
        {
            println("Failed to create file");
            return false;
        }
        file.close();
        println("File created successfully");
        return true;
    }

    String ReadFile(const String &fileName)
    {
        println("Reading file " + fileName);
        if(!SPIFFS.exists("/" + fileName))
        {
            println("Source file does not exist");
            return "";
        }
        String content;
        File file = SPIFFS.open("/" + fileName);
        if (!file)
        {
            println("Failed to open file for reading");
            return "";
        }
        
        while (file.available())
        {
            content += file.readString();
        }
        file.close();
        return content;
    }

    bool WriteFile(const String &fileName, const String &message, bool createFileIfNotExists)
    {
        println("Writing file " + fileName);
        if(!SPIFFS.exists("/" + fileName))
        {
            if (createFileIfNotExists)
            {
                if (!CreateFile(fileName))
                    return false;
            }
            else
            {
                println("Source file does not exist");
                return false;
            }
        }
        File file = SPIFFS.open("/" + fileName, FILE_WRITE);
        if (!file)
        {
            println("Failed to open file for writing");
            return false;
        }
        bool written = file.print(message) == message.length();
        println(written ? "File written" : "Write failed");
        file.close();
        return written;
    }

    bool AppendFile(const String &fileName, const vector<char> &message, bool createFileIfNotExists)
    {
        println("Appending to file " + fileName);

        if(!SPIFFS.exists("/" + fileName))
        {
            if (createFileIfNotExists)
            {
                if (!CreateFile(fileName))
                    return false;
            }
            else
            {
                println("Source file does not exist");
                return false;
            }
        }
        File file = SPIFFS.open("/" + fileName, FILE_APPEND);
        if (!file)
        {
            println("Failed to open file for appending");
            return false;
        }
        bool appended = file.write(reinterpret_cast<const uint8_t *>(message.data()), message.size()) == message.size();
        println(appended ? "Message appended" : "Append failed");
        file.close();
        return appended;
    }

    bool AppendFile(const String &fileName, const String &message, bool createFileIfNotExists)
    {
        println("Appending to file " + fileName);

        if(!SPIFFS.exists("/" + fileName))
        {
            if (createFileIfNotExists)
            {
                if (!CreateFile(fileName))
                    return false;
            }
            else
            {
                println("Source file does not exist");
                return false;
            }
        }
        File file = SPIFFS.open("/" + fileName, FILE_APPEND);
        if (!file)
        {
            println("Failed to open file for appending");
            return false;
        }
        bool appended = file.print(message) == message.length();
        println(appended ? "Message appended" : "Append failed");
        file.close();
        return appended;
    }

    bool RenameFile(const String &path1, const String &path2)
    {
        println("Renaming file %s to file %s", path1.c_str(), path2.c_str());

        if(!SPIFFS.exists("/" + path1))
        {
            println("Source file does not exist");
            return false;
        }
        if(SPIFFS.exists("/" + path2))
        {
            println("Target file already exists");
            return false;
        }
        if (SPIFFS.rename("/" + path1, "/" + path2))
        {
            println("File renamed");
            return true;
        }
        println("Rename failed");
        return false;
    }

    bool DeleteFile(const String &fileName)
    {
        println("Deleting file %s", fileName.c_str());
        
        if(!SPIFFS.exists("/" + fileName))
        {
            println("Source file does not exist");
            return false;
        }
        if (SPIFFS.remove("/" + fileName))
        {
            println("File deleted");
            return true;
        }
        println("Delete failed");
        return false;
    }

    void TestFileIO(const String &fileName)
    {
        println("Testing file I/O with " + fileName);

        static uint8_t buf[512];
        size_t len = 0;
        File file = SPIFFS.open("/" + fileName, FILE_WRITE);
        if (!file)
        {
            println("- failed to open file for writing");
            return;
        }

        size_t i;
        print("- writing");
        uint32_t start = millis();
        for (i = 0; i < 2048; i++)
        {
            if ((i & 0x001F) == 0x001F)
            {
                print(".");
            }
            file.write(buf, 512);
        }
        println("");
        uint32_t end = millis() - start;
        printf(" - %u bytes written in %lu ms\r\n", 2048 * 512, end);
        file.close();

        file = SPIFFS.open("/" + fileName);
        start = millis();
        end = start;
        i = 0;
        if (file && !file.isDirectory())
        {
            len = file.size();
            size_t flen = len;
            start = millis();
            print("- reading");
            while (len)
            {
                size_t toRead = len;
                if (toRead > 512)
                {
                    toRead = 512;
                }
                file.read(buf, toRead);
                if ((i++ & 0x001F) == 0x001F)
                {
                    print(".");
                }
                len -= toRead;
            }
            println("");
            end = millis() - start;
            println("- %u bytes read in %lu ms\r\n", flen, end);
            file.close();
        }
        else
        {
            println("- failed to open file for reading");
        }
    }

    bool HandleCommand(Command cmdTmp)
    {
        if (cmdTmp.cmdEquals("SPIFFSFormat"))
        {
            // SPIFFSFormat
            FormatFS();
        }
        else if(cmdTmp.cmdEquals("SPIFFSListFiles"))
        {
            // SPIFFSListFiles:<filter>
            ListFiles(String(cmdTmp.dataStr1));
        }
        else if(cmdTmp.cmdEquals("SPIFFSCreateFile"))
        {
            // SPIFFSCreateFile:<fileName>
            CreateFile(String(cmdTmp.dataStr1));
        }
        else if(cmdTmp.cmdEquals("SPIFFSReadFile"))
        {
            // SPIFFSReadFile:<fileName>
            println(ReadFile(String(cmdTmp.dataStr1)));
        }
        else if(cmdTmp.cmdEquals("SPIFFSWriteFile"))
        {
            // SPIFFSWriteFile:<fileName>:<msg>
            // SPIFFSWriteFile:<fileName>:<msg>:1
            bool createFileIfNotExists = cmdTmp.size > 0 ? cmdTmp.data[0] : false;
            WriteFile(String(cmdTmp.dataStr1), String(cmdTmp.dataStr2), createFileIfNotExists);
        }
        else if(cmdTmp.cmdEquals("SPIFFSAppendFile"))
        {
            // SPIFFSAppendFile:<fileName>:<msg>
            // SPIFFSAppendFile:<fileName>:<msg>:1
            bool createFileIfNotExists = cmdTmp.size > 0 ? cmdTmp.data[0] : false;
            AppendFile(String(cmdTmp.dataStr1), String(cmdTmp.dataStr2), createFileIfNotExists);
        }
        else if(cmdTmp.cmdEquals("SPIFFSRenameFile"))
        {
            // SPIFFSRenameFile:<oldFileName>:<newFileName>
            RenameFile(String(cmdTmp.dataStr1), String(cmdTmp.dataStr2));
        }
        else if(cmdTmp.cmdEquals("SPIFFSDeleteFile"))
        {
            // SPIFFSDeleteFile:<fileName>
            DeleteFile(String(cmdTmp.dataStr1));
        }
        else
        {
            println("Not a SPIFFS command !");            
            return false;
        }
        return true;
    }
    
    void PrintCommandHelp()
    {
        println("SPIFFS Command Help");
        println(" > SPIFFSFormat");
        println("      Format SPIFFS");
        println(" > SPIFFSListFiles:<filter>");
        println("      List files in the SPIFFS filesystem, filter is optional");
        println(" > SPIFFSCreateFile:<fileName>");
        println("      Create a new file");
        println(" > SPIFFSReadFile:<fileName>");
        println("      Read file contents");
        println(" > SPIFFSWriteFile:<fileName>:<msg>:<createIfNotExists>");
        println("      Write message to file");
        println(" > SPIFFSAppendFile:<fileName>:<msg>:<createIfNotExists>");
        println("      Append message to file");
        println(" > SPIFFSAppendASCII:<fileName>:<characterCount>");
        println("      Append exactly characterCount ASCII characters");
        println("      Send the body immediately after the header newline; body has no terminator");
        println(" > SPIFFSRenameFile:<oldFileName>:<newFileName>");
        println("      Rename the <oldFileName> file to <newFileName>");
        println(" > SPIFFSDeleteFile:<fileName>");
        println("      Delete the specified file");
        println();
    }
}