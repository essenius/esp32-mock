// Copyright 2022-2026 Rik Essenius
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
#include <cstring> // for strlen

class String {
public:
    // ReSharper disable once CppNonExplicitConvertingConstructor -- done on purpose to be able to use initialization
    String(const char* value) : _string(value) {}

	String() : String("") {}

    unsigned char concat(const char* data, size_t len) { _string.append(data, len); return 1; }
    String& concat(char c) { _string.push_back(c); return *this; }
    String& concat(const String& other) { _string.append(other._string); return *this; }

    bool isEmpty() const { return _string.empty(); }

    void reserve(unsigned int n) { _string.reserve(n); }

    int toInt() const {
        try { return std::stoi(_string); }
        catch (...) { return 0; } // NOSONAR - existing behavior of the Arduino String class
    }
    
    const char* c_str() const { return _string.c_str(); }

    size_t length() const { return _string.size(); }
    size_t size() const { return _string.size(); }

    String& operator+=(const String& other) { return concat(other); }
    String& operator+=(const char* s) { concat(s, strlen(s)); return *this; }
    String& operator+=(char c) { return concat(c); }

    friend bool operator==(const String& lhs, const char* rhs) { return lhs._string == rhs; }
    friend bool operator!=(const String& lhs, const char* rhs) { return lhs._string != rhs; }
    friend bool operator==(const String& lhs, const String& rhs) { return lhs._string == rhs._string; }
    friend bool operator!=(const String& lhs, const String& rhs) { return lhs._string != rhs._string; }


private:
    std::string _string;
};

#endif
