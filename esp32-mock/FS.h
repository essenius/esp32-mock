#ifndef FS_H
#define FS_H
#include "StringArduino.h"
#include <map>
#include <vector>

enum SeekMode {
    SeekSet = 0,
    SeekCur = 1,
    SeekEnd = 2
};

class Dir {
public:
    bool next();
    String fileName() const;
    size_t fileSize() const;
    bool _first = true;
    explicit Dir(const std::map<std::string, size_t>& files);
    std::map<std::string, size_t> _files;
    std::map<std::string, size_t>::iterator _iterator;
};

class File {
public:
    File(const char* path, const char* mode);
    File() = default;
    bool _exists = false;
    explicit operator bool() const;
    int available() const;
    void close();
    size_t position() const;
    void seek(int offset, SeekMode mode);
    int size() const;
    String readString();
    void write(const char* str, size_t length);

    // testing only
    static void deleteFiles();
    static void defineFile(const char* path, const char* content);
    static std::map<std::string, size_t> filesInFolder(const char* folder);
private:
    size_t _currentPosition = 0;
    std::string _path;
    std::string _content;
    int _mode = 0;
    static constexpr int In = 1;
    static constexpr int Out = 2;
    static constexpr int Append = 4;
    static std::map <std::string, std::string> _fileMap;
};

class FS {
 public:
     void begin() { _started = true; }
     File open(const char* path, const char* mode);
     Dir openDir(const char* folder);
private:
    bool _started = false;
};

extern FS SPIFFS;
#endif
