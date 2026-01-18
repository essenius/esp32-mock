// Copyright 2024-2026 Rik Essenius
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
// ReSharper disable CppInconsistentNaming

#include "FS.h"

#include <algorithm>
#include "StringArduino.h"

FS SPIFFS;

std::map<std::string, FileData> File::_fileMap;

File::operator bool() const {
	return _valid;
}

size_t File::available() const {
	return _currentPosition >= size() ? 0 : size() - _currentPosition;
}

void File::close() {
	if (_valid && (_mode & kOut) == kOut) {
		_fileMap[_path] = _content;
	}
	_currentPosition = 0;
	_content.clear();
	_path = "";
	_exists = false;
	_valid = false;
}

int File::read() {
	if ((_mode & kIn) == 0)
		return -1;
	if (_currentPosition >= size())
		return -1;
	return _content[_currentPosition++];
}

size_t File::read(uint8_t* buffer, const size_t length) {
	if ((_mode & kIn) == 0)
		return 0;
	size_t bytesRead = 0;
	while (bytesRead < length && _currentPosition < size()) {
		buffer[bytesRead] = _content[_currentPosition];
		bytesRead++;
		_currentPosition++;
	}
	return bytesRead;
}

void File::seek(const int offset, const SeekMode mode) {
	long newPos;
	switch (mode) {
		case SeekSet: newPos = offset;
			break;
		case SeekCur: newPos = static_cast<long>(_currentPosition) + offset;
			break;
		case SeekEnd:
		default: 
			newPos = static_cast<long>(size()) + offset;
	}

	newPos = std::max<long>(newPos, 0);
	if (newPos > static_cast<long>(size())) {
		_currentPosition = size();
	}
	else {
		_currentPosition = static_cast<size_t>(newPos);
	}
}

size_t File::size() const {
	return _content.size();
}

String File::readString() {
	if ((_mode & kIn) == 0)
		return "";
	if (_currentPosition >= size())
		return "";

	const auto length = size() - _currentPosition;

	String result;
	result.reserve(static_cast<unsigned int>(length));

	result.concat(reinterpret_cast<const char*>(&_content[_currentPosition]), length);
	_currentPosition = _content.size();
	return result;
}

size_t File::write(const char character) {
	return write(&character, 1);
}

size_t File::write(const uint8_t* buffer, const size_t size) {
	if ((_mode & kAppend) == kAppend) {
		_content.insert(_content.end(), buffer, buffer + size);
		_currentPosition = _content.size();
		return size;
	}
	if ((_mode & kOut) == 0) return 0;

	if (_currentPosition + size > _content.size()) {
		_content.resize(_currentPosition + size);
	}
	for (size_t i = 0; i < size; i++) {
		_content[_currentPosition + i] = buffer[i];
	}
	_currentPosition += size;
	return size;
}

size_t File::write(const char* str, const size_t length) {
	return write(reinterpret_cast<const uint8_t*>(str), length);
}

void File::testDeleteFiles() {
	_fileMap.clear();
}

void File::testDefineFile(const char* path, const char* content) {
	_fileMap[path].assign(content, content + strlen(content));
}

size_t File::position() const { return _currentPosition; }

std::map<std::string, size_t> File::testGetFilesInFolder(const char* folder) {
	std::map<std::string, size_t> result;
	for (const auto& pair : _fileMap) {
		if (pair.first.rfind(folder, 0) == 0) {
			result[pair.first] = static_cast<int>(pair.second.size());
		}
	}
	return result;
}

bool Dir::next() {
	if (_first) {
		_iterator = _files.begin();
		_first = false;
	}
	else {
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

Dir::Dir(const std::map<std::string, size_t>& files) : _files(files) {
	_iterator = _files.begin();
}

File::File(const char* path, const char* mode) {
	_path = path;
	_currentPosition = 0;
	auto truncate = false;
	_mode = 0;
	if (strchr(mode, 'r')) {
		_mode |= kIn;
	}
	if (strchr(mode, 'w')) {
		_mode |= kOut;
		truncate = true;
	}
	if (strchr(mode, 'a')) {
		_mode |= kOut | kAppend;
	}
	if (strchr(mode, '+')) {
		_mode |= kIn | kOut;
	}

	// if the file does not exist, or we need to truncate, clear the file
	if (_fileMap.find(path) == _fileMap.end() || truncate) {
		_content.clear();
		_exists = false;
	}
	else {
		_content = _fileMap[path];
		if ((_mode & kAppend) == kAppend) {
			_currentPosition = _content.size();
		}
		_exists = true;
	}
	_valid = _exists || (_mode & kOut) == kOut;
}

File FS::open(const char* path, const char* mode) {
	if (!_started)
		return {};
	return {path, mode};
}

Dir FS::openDir(const char* folder) {
	const auto files = File::testGetFilesInFolder(folder);
	return Dir(files);
}
