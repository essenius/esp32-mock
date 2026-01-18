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

#ifndef HEADER_WIFI
#define HEADER_WIFI

#include "WiFiCommon.h"

class WiFiClass : public WiFiCommon {
#ifdef  ARDUINO_ARCH_ESP8266
public:
    bool hostname(const char* name) {
        strncpy(_name, name, sizeof(_name) - 1);
        _name[sizeof(_name) - 1] = 0;
        return true;
    }

    String hostname() const { return { _name }; }
#endif
};

// ReSharper disable CppInconsistentNaming

extern WiFiClass WiFi;
#endif