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

// ReSharper disable CppMemberFunctionMayBeStatic - mimicking existing interface
#include "EEPROM.h"
#include "ESP.h"

const char* EEPROMClass::FileName = "EEPROM.bin";
char EEPROMClass::_committedContent[MaxSize] = "";
char EEPROMClass::_uncommittedContent[MaxSize] = "";


void EEPROMClass::reset() {
    (void)std::remove(FileName);
}

void EEPROMClass::begin(int /*maxSize*/) {
    std::ifstream myFile;
    myFile.open(FileName, std::ios::binary);
    if (myFile.fail()) return;
    // sync uncommitted and committed content
    myFile.read(_uncommittedContent, MaxSize);
    commit();
    myFile.close();
}

bool EEPROMClass::commit() {
    std::memcpy(_committedContent, _uncommittedContent, MaxSize);
    return true;
}

// note that read/write operate on committed content while put/get operate on uncommitted content

byte EEPROMClass::read(const int address) {
    return static_cast<byte>(_committedContent[address]);
}

void EEPROMClass::write(const int address, const byte value) {
    _committedContent[address] = static_cast<char>(value);
}

void EEPROMClass::end() {
    std::ofstream myFile;
    myFile.open(FileName, std::ios::binary| std::ofstream::out | std::ofstream::trunc);
    myFile.write(_committedContent, MaxSize);
    myFile.close();
} 