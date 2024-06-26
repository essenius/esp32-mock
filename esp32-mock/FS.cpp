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

// Mimicking existing interface
// ReSharper disable CppMemberFunctionMayBeStatic 
// ReSharper disable CppMemberFunctionMayBeConst

#include "FS.h"
#include "StringArduino.h"

FS SPIFFS;

std::map <std::string, std::string> File::_fileMap;


File::operator bool() const {
    return _exists || (_mode & File::Out) == File::Out;
}

size_t File::available() const {
    return size() - _currentPosition;
}

void File::close() {
    if ((_mode & File::Out) == File::Out) {
        _fileMap[_path] = _content;
    }
    _currentPosition = 0;
    _content = "";
    _path = "";
    _exists = false;
}

void File::seek(const int offset, const SeekMode mode) {
    switch(mode) {
    case SeekSet: _currentPosition = offset; break;
    case SeekCur: _currentPosition += offset; break;
    case SeekEnd: _currentPosition = size() + offset;
    }
}

size_t File::size() const {
    return _content.length();
}

String File::readString() {
    if ((_mode & File::In) == 0) return "";
    const auto oldPosition = _currentPosition;
    _currentPosition = size();
    return { _content.substr(oldPosition).c_str() };
}

void File::write(const char* str, const size_t length) {
    if ((_mode & File::Out) == 0) return;
    if ((_mode & File::Append) == File::Append) {
        _content += str;
    } else {
        _content.replace(_currentPosition, length, str);
    }
}

void File::deleteFiles() {
    _fileMap.clear();
}

void File::defineFile(const char* path, const char* content) {
    _fileMap[path] = content;
}

size_t File::position() const { return _currentPosition; }

std::map<std::string, size_t> File::filesInFolder(const char* folder) {
    std::map<std::string, size_t> result;
    for (const auto& pair: _fileMap) {
        if(pair.first.rfind(folder, 0) == 0) {
            result[pair.first] = static_cast<int>(pair.second.length());
        }
    }
    return result;
}

bool Dir::next() {
    if (_first) {
        _iterator = _files.begin();
        _first = false;
    } else {
        ++_iterator;
    }
    return _iterator != _files.end();
}

String Dir::fileName() const {
    return {_iterator->first.c_str()};
}

size_t Dir::fileSize() const {
    return _iterator->second;
}

Dir::Dir(const std::map<std::string, size_t>& files): _files(files) {
    _iterator = _files.begin();
}

File::File(const char* path, const char* mode) {
    _path = path;
    _currentPosition = 0;
    auto truncate = false;
    switch (mode[0]) {
    case 'r': _mode = File::In; break;
    case 'w': _mode = File::Out; truncate = true; break;
    case 'a': _mode = File::Out | File::Append; break;
    default: 
        _mode = 0;
        _exists = false;
        _content = "";
        return;
    }
    switch (mode[1]) {
    case '+': _mode |= File::In | File::Out; break;
    case 0: break;
    default:;
    }
    // if the file does not exist, or we need to truncate, clear the file
    if (_fileMap.find(path) == _fileMap.end() || truncate) {
        _content = "";
        _exists = (_mode & File::Out) == File::Out;
    } else {
        _content = _fileMap[path];
        if ((_mode & File::Append) == File::Append) {
            _currentPosition = _content.length();
        }
        _exists = true;
    }
}

File FS::open(const char* path, const char* mode) {
    if (!_started) return {};
    return {path, mode};
}

Dir FS::openDir(const char* folder) {
    const auto files = File::filesInFolder(folder);
    return Dir(files);
}

