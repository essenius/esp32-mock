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

// Mock implementation of the EEPROM library for unit testing (not targeting the ESP8266)

// ReSharper disable CppInconsistentNaming -- mocking existing interface

#ifndef HEADER_EEPROM
#define HEADER_EEPROM
#include <fstream>

#include "ESP.h"


class EEPROMClass {
public:
    static void begin(int maxSize);
    static byte read(int address);
    static void write(int address, byte value);
    static void end();
private:
    static constexpr int MaxSize = 512;
    static const char* FileName;
    static char _content[MaxSize];
};

static EEPROMClass EEPROM;

#endif