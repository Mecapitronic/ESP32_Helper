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

    void ListFiles(String filter)
    {
        print("Listing files ");
        filter != "" ? println(" with filter : <", filter, ">") : println(" :");

        File root = SPIFFS.open("/");
        if (!root)
        {
            println("Failed to open root directory");
            return;
        }

        File file = root.openNextFile();
        println("  FILE: \t SIZE: ");
        while (file)
        {
            if (filter == "" || (filter != "" && String(file.name()).indexOf(filter) != -1))
            {
                print("  ", String(file.name()));
                println(" \t ", String(file.size()));
            }
            file = root.openNextFile();
        }
        root.close();
    }

    void CreateFile(String fileName)
    {
        println("Creating file: ", fileName);
        
        if(SPIFFS.exists("/" + fileName))
        {
            println("File already exists");
            return;
        }

        File file = SPIFFS.open("/" + fileName, FILE_WRITE);
        if (!file)
        {
            println("Failed to create file");
            return;
        }
        file.close();
        println("File created successfully");
    }

    String ReadFile(String fileName)
    {
        println("Reading file: ", fileName);
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

    void WriteFile(String fileName, String message, bool createFileIfNotExists)
    {
        println("Writing file: ", fileName);
        if(!SPIFFS.exists("/" + fileName))
        {
            if (createFileIfNotExists)
            {
                CreateFile(fileName);
            }
            else
            {
                println("Source file does not exist");
                return;
            }
        }
        File file = SPIFFS.open("/" + fileName, FILE_WRITE);
        if (!file)
        {
            println("Failed to open file for writing");
            return;
        }
        if (file.print(message))
        {
            println("File written");
        }
        else
        {
            println("Write failed");
        }
        file.close();
    }

    void AppendFile(String fileName, String message, bool createFileIfNotExists)
    {
        println("Appending to file: ", fileName);

        if(!SPIFFS.exists("/" + fileName))
        {
            if (createFileIfNotExists)
            {
                CreateFile(fileName);
            }
            else
            {
                println("Source file does not exist");
                return;
            }
        }
        File file = SPIFFS.open("/" + fileName, FILE_APPEND);
        if (!file)
        {
            println("Failed to open file for appending");
            return;
        }
        if (file.print(message))
        {
            println("Message appended");
        }
        else
        {
            println("Append failed");
        }
        file.close();
    }

    void RenameFile(String path1, String path2)
    {
        print("Renaming file ", path1);
        println(" to file ", path2);

        if(!SPIFFS.exists("/" + path1))
        {
            println("Source file does not exist");
            return;
        }
        if(SPIFFS.exists("/" + path2))
        {
            println("Target file already exists");
            return;
        }
        if (SPIFFS.rename("/" + path1, "/" + path2))
        {
            println("File renamed");
        }
        else
        {
            println("Rename failed");
        }
    }

    void DeleteFile(String fileName)
    {
        println("Deleting file: ", fileName);
        
        if(!SPIFFS.exists("/" + fileName))
        {
            println("Source file does not exist");
            return;
        }
        if (SPIFFS.remove("/" + fileName))
        {
            println("File deleted");
        }
        else
        {
            println("Delete failed");
        }
    }

    void TestFileIO(String fileName)
    {
        printf("Testing file I/O with %s\r\n", fileName);

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
            printf("- %u bytes read in %lu ms\r\n", flen, end);
            file.close();
        }
        else
        {
            println("- failed to open file for reading");
        }
    }

    void HandleCommand(Command cmdTmp)
    {
        if (cmdTmp.cmd == "SPIFFSFormat")
        {
            // SPIFFSFormat
            FormatFS();
        }
        else if(cmdTmp.cmd == "SPIFFSListFiles")
        {
            // SPIFFSListFiles:<filter>
            ListFiles(cmdTmp.dataStr1);
        }
        else if(cmdTmp.cmd == "SPIFFSCreateFile")
        {
            // SPIFFSCreateFile:<fileName>
            CreateFile(cmdTmp.dataStr1);
        }
        else if(cmdTmp.cmd == "SPIFFSReadFile")
        {
            // SPIFFSReadFile:<fileName>
            println(ReadFile(cmdTmp.dataStr1));
        }
        else if(cmdTmp.cmd == "SPIFFSWriteFile")
        {
            // SPIFFSWriteFile:<fileName>:<msg>
            // SPIFFSWriteFile:<fileName>:<msg>:1
            bool createFileIfNotExists = cmdTmp.size > 0 ? cmdTmp.data[0] : false;
            WriteFile(cmdTmp.dataStr1, cmdTmp.dataStr2, createFileIfNotExists);
        }
        else if(cmdTmp.cmd == "SPIFFSAppendFile")
        {
            // SPIFFSAppendFile:<fileName>:<msg>
            // SPIFFSAppendFile:<fileName>:<msg>:1
            bool createFileIfNotExists = cmdTmp.size > 0 ? cmdTmp.data[0] : false;
            AppendFile(cmdTmp.dataStr1, cmdTmp.dataStr2, createFileIfNotExists);
        }
        else if(cmdTmp.cmd == "SPIFFSRenameFile")
        {
            // SPIFFSRenameFile:<oldFileName>:<newFileName>
            RenameFile(cmdTmp.dataStr1, cmdTmp.dataStr2);
        }
        else if(cmdTmp.cmd == "SPIFFSDeleteFile")
        {
            // SPIFFSDeleteFile:<fileName>
            DeleteFile(cmdTmp.dataStr1);
        }
        else
        {
            println("Not a SPIFFS command !");            
            PrintCommandHelp();
        }
    }
    
    void PrintCommandHelp()
    {
        println("SPIFFS Command Help :");
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
        println(" > SPIFFSRenameFile:<oldFileName>:<newFileName>");
        println("      Rename the <oldFileName> file to <newFileName>");
        println(" > SPIFFSDeleteFile:<fileName>");
        println("      Delete the specified file");
        println();
    }
}