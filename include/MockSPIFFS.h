#ifndef MOCK_SPIFFS_H
#define MOCK_SPIFFS_H

#include <Arduino.h>
#include <vector>

// Simple mock of SPIFFS for simulation: stores files in a vector and
// provides a File-like API used by FileSystem_Helper.

// Provide FILE_WRITE/FILE_APPEND if missing (used by existing code)
#ifndef FILE_WRITE
#define FILE_WRITE 1
#endif
#ifndef FILE_APPEND
#define FILE_APPEND 2
#endif

struct MockFileEntry {
    String path;
    String content;
};

class MockSPIFFSClass; // forward

class MockFile {
public:
    MockFile() : owner(nullptr), idx(-1), isDir(false), pos(0), writable(false), valid(false) {}
    MockFile(MockSPIFFSClass* owner, int index, bool writable=false, bool isDir=false);

    size_t print(const String& s);
    size_t println(const String& s);
    String readString();
    String readStringUntil(char c);
    int available();
    size_t size() const;
    void close();
    bool isDirectory() const;
    MockFile openNextFile();
    const char* name() const;
    size_t read(uint8_t* buffer, size_t len);
    size_t write(const uint8_t* buffer, size_t len);
    operator bool() const { return valid; }

private:
    MockSPIFFSClass* owner;
    int idx; // index into owner's files vector; -2 for directory handle
    bool isDir;
    int pos; // read position or dir iterator
    bool writable;
    bool valid;
};

class MockSPIFFSClass {
public:
    MockSPIFFSClass() {}

    bool begin(bool formatOnFail = true);
    bool format();

    // open with mode 'r','w','a' or with flags FILE_WRITE/FILE_APPEND
    MockFile open(const String& path, const char* mode = "r");
    MockFile open(const String& path, int flags);

    bool exists(const String& path) const;
    bool remove(const String& path);
    bool rename(const String& from, const String& to);

    // helper for tests: preload a file
    void preload(const String& path, const String& content);

private:
    std::vector<MockFileEntry> files;
    int indexOf(const String& path) const;

    friend class MockFile;
};

// provide fs::FS alias used by existing code
namespace fs { using FS = MockSPIFFSClass; }

// instance used when SIMULATOR is defined (FileSystem_Helper.h maps SPIFFS to MockSPIFFS)
extern MockSPIFFSClass MockSPIFFS;

#endif