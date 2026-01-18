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

// Mock implementation of the EEPROM library for unit testing (not targeting the ESP8266)

// ReSharper disable CppMemberFunctionMayBeStatic - mimicking existing interface
// ReSharper disable CppMemberFunctionMayBeConst

// ReSharper disable CppInconsistentNaming
#include "EEPROM.h"
#include "ESP.h"

char EEPROMClass::_committedContent[kMaxSize] = "";
char EEPROMClass::_uncommittedContent[kMaxSize] = "";

void EEPROMClass::reset() {
    (void)std::remove(kFileName);
}

void EEPROMClass::begin(int /*maxSize*/) {
    std::ifstream myFile;
    myFile.open(kFileName, std::ios::binary);
    if (myFile.fail()) {
        memset(_uncommittedContent, 0, kMaxSize);
    }
    // sync uncommitted and committed content
    myFile.read(_uncommittedContent, kMaxSize);
    commit();
    myFile.close();
}

bool EEPROMClass::commit() {
    std::memcpy(_committedContent, _uncommittedContent, kMaxSize);
    _isDirty = false;
    return true;
}

bool EEPROMClass::isDirty() {
    return _isDirty;
}

// note that read/write operate on committed content while put/get operate on uncommitted content

byte EEPROMClass::read(const int address) {
    return static_cast<byte>(_uncommittedContent[address]);
}

void EEPROMClass::write(const int address, const byte value) {
    if (_uncommittedContent[address] == static_cast<char>(value)) return;
    _uncommittedContent[address] = static_cast<char>(value);
    _isDirty = true;
}

void EEPROMClass::end() {
    std::ofstream myFile;
    if (_isDirty) commit();
    myFile.open(kFileName, std::ios::binary | std::ofstream::out | std::ofstream::trunc);
    myFile.write(_committedContent, kMaxSize);
    myFile.close();
} 

EEPROMClass EEPROM;
