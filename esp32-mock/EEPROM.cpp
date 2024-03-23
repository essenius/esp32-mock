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

#include "EEPROM.h"
#include "ESP.h"

const char* EEPROMClass::FileName = "EEPROM.bin";
char EEPROMClass::_content[512] = "";


void EEPROMClass::begin(int /*maxSize*/) {
    std::ifstream myFile;
    myFile.open(FileName, std::ios::binary);
    if (myFile.fail()) {
        return;
    }
    myFile.read(_content, MaxSize);
    myFile.close();
}

byte EEPROMClass::read(const int address) {
    return static_cast<byte>(_content[address]);
}

void EEPROMClass::write(const int address, const byte value) {
    _content[address] = static_cast<char>(value);
}

void EEPROMClass::end() {
    std::ofstream myFile;
    myFile.open(FileName, std::ios::binary| std::ofstream::out | std::ofstream::trunc);
    myFile.write(_content, MaxSize);
    myFile.close();
} 