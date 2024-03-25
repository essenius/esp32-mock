// Copyright 2024 Rik Essenius
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
// except in compliance with the License. You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and limitations under the License.

// Mock implementation of the ESP library for unit testing (not targeting the ESP32)

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
    size_t available() const;
    void close();
    size_t position() const;
    void seek(int offset, SeekMode mode);
    size_t size() const;
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
