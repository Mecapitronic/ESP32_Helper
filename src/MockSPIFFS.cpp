#include "MockSPIFFS.h"

// global instance
MockSPIFFSClass MockSPIFFS;

// ---------------- MockFile ----------------
MockFile::MockFile(MockSPIFFSClass* owner_, int index, bool writable_, bool isDir_)
    : owner(owner_), idx(index), isDir(isDir_), pos(0), writable(writable_), valid(true)
{
    if (!owner_) { valid = false; return; }
    if (!isDir_ && (idx < 0 || idx >= (int)owner_->files.size())) { valid = false; }
}

size_t MockFile::print(const String& s) {
    if (!valid || !writable) return 0;
    owner->files[idx].content += s;
    return s.length();
}

size_t MockFile::println(const String& s) {
    return print(s + "\n");
}

String MockFile::readString() {
    if (!valid) return String();
    String &c = owner->files[idx].content;
    String out = c.substring(pos);
    pos = c.length();
    return out;
}

String MockFile::readStringUntil(char c) {
    if (!valid) return String();
    String &ct = owner->files[idx].content;
    int idxFound = ct.indexOf(c, pos);
    if (idxFound < 0) {
        String res = ct.substring(pos);
        pos = ct.length();
        return res;
    }
    String res = ct.substring(pos, idxFound);
    pos = idxFound + 1;
    return res;
}

int MockFile::available() {
    if (!valid) return 0;
    return (int)owner->files[idx].content.length() - pos;
}

size_t MockFile::size() const {
    if (!valid) return 0;
    return owner->files[idx].content.length();
}

void MockFile::close() {
    // no-op
}

bool MockFile::isDirectory() const {
    return isDir;
}

MockFile MockFile::openNextFile() {
    if (!isDir) return MockFile();
    int total = (int)owner->files.size();
    if (pos >= total) return MockFile();
    int fileIdx = pos++;
    return MockFile(owner, fileIdx, false, false);
}

const char* MockFile::name() const {
    if (!valid) return "";
    return owner->files[idx].path.c_str();
}

size_t MockFile::read(uint8_t* buffer, size_t len) {
    if (!valid) return 0;
    String &c = owner->files[idx].content;
    if (pos >= (int)c.length()) return 0;
    size_t toRead = len;
    if (pos + (int)toRead > (int)c.length()) toRead = c.length() - pos;
    for (size_t i = 0; i < toRead; ++i) buffer[i] = (uint8_t)c[pos + i];
    pos += toRead;
    return toRead;
}

size_t MockFile::write(const uint8_t* buffer, size_t len) {
    if (!valid || !writable) return 0;
    String &c = owner->files[idx].content;
    for (size_t i = 0; i < len; ++i) c += (char)buffer[i];
    return len;
}

// ---------------- MockSPIFFSClass ----------------
int MockSPIFFSClass::indexOf(const String& path) const {
    for (size_t i = 0; i < files.size(); ++i) if (files[i].path == path) return (int)i;
    return -1;
}

    bool MockSPIFFSClass::begin(bool formatOnFail){return true;}
    bool MockSPIFFSClass::format(){return true;}

MockFile MockSPIFFSClass::open(const String& path, const char* mode) {
    char m = (mode && mode[0]) ? mode[0] : 'r';
    if (path == "/") {
        MockFile dir(this, -2, false, true);
        return dir;
    }
    int idx = indexOf(path);
    if (m == 'r') {
        if (idx < 0) return MockFile();
        return MockFile(this, idx, false, false);
    } else if (m == 'w') {
        if (idx < 0) { files.push_back({path, ""}); idx = (int)files.size() - 1; }
        else files[idx].content = "";
        return MockFile(this, idx, true, false);
    } else if (m == 'a') {
        if (idx < 0) { files.push_back({path, ""}); idx = (int)files.size() - 1; }
        return MockFile(this, idx, true, false);
    }
    return MockFile();
}

MockFile MockSPIFFSClass::open(const String& path, int flags) {
#if defined(FILE_APPEND) && defined(FILE_WRITE)
    if (flags == FILE_APPEND) return open(path, "a");
    if (flags == FILE_WRITE) return open(path, "w");
#endif
    if (flags != 0) return open(path, "w");
    return open(path, "r");
}

bool MockSPIFFSClass::exists(const String& path) const {
    return indexOf(path) >= 0;
}

bool MockSPIFFSClass::remove(const String& path) {
    int idx = indexOf(path);
    if (idx < 0) return false;
    files.erase(files.begin() + idx);
    return true;
}

bool MockSPIFFSClass::rename(const String& from, const String& to) {
    int idx = indexOf(from);
    if (idx < 0) return false;
    int idxTo = indexOf(to);
    if (idxTo >= 0) {
        files[idxTo].content = files[idx].content;
        files.erase(files.begin() + idx);
    } else {
        files[idx].path = to;
    }
    return true;
}

void MockSPIFFSClass::preload(const String& path, const String& content) {
    int idx = indexOf(path);
    if (idx < 0) files.push_back({path, content});
    else files[idx].content = content;
}

// preload only texte.txt
static bool _mockspiffs_preload = []() {
    MockSPIFFS.preload("/texte.txt", "Hello World\n");
    String map_json = R"(
{
  "map": {
    "color": "jaune",
    "vertices": [
      { "id": 0, "point": { "x": 0, "y": 0 } },
      { "id": 1, "point": { "x": 250, "y": 250 } },
      { "id": 2, "point": { "x": 500, "y": 250 } }
    ],
    "segments": [
      { "id": 0, "start": { "x": 0, "y": 1500 }, "end": { "x": 1500, "y": 1500 } },
      { "id": 1, "start": { "x": 1500, "y": 1500 }, "end": { "x": 1500, "y": 0 } }
    ],
    "circles": [
      { "id": 0, "center": { "x": 750, "y": 750 }, "radius": 100 },
      { "id": 1, "center": { "x": 1200, "y": 300 }, "radius": 150 }
    ]
  }
}
)";
    MockSPIFFS.preload("/map.json", map_json);

    return true;
}();