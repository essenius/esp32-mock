// Copyright 2022-2024 Rik Essenius
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
// except in compliance with the License. You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and limitations under the License.

// Mock implementation of the Arduino String class for unit testing (not targeting the ESP32)

// Disabling warnings caused by mimicking existing interface
// ReSharper disable CppInconsistentNaming

#ifndef HEADER_STRING_ARDUINO
#define HEADER_STRING_ARDUINO

#include <string>
#include <cstring> 

class String {
public:
    // ReSharper disable once CppNonExplicitConvertingConstructor -- done on purpose to be able to use initialization
    String(const char* value) : _string(value) {}

    bool operator==(const char* other) const { return strcmp(_string.c_str(), other) == 0; }
    bool operator!=(const char* other) const { return strcmp(_string.c_str(), other) != 0; }
    int toInt() const { return std::stoi(_string); }
    const char* c_str() const { return _string.c_str(); }

private:
    std::string _string;
};

#endif
