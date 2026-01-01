// Copyright 2025 Rik Essenius
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
// except in compliance with the License. You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and limitations under the License.


#ifndef HEADER_ESP8266_WIFI
#define HEADER_ESP8266_WIFI
#include "WiFiCommon.h"
#include "StringArduino.h"

class WiFiClass : public WiFiCommon {
    public:
        bool hostname(const char* name);
        String hostname();
};

extern WiFiClass WiFi;

namespace BearSSL {
    class X509List {
    public:
        explicit X509List(const char* pem) {
            (void)pem; // ignore certificate content
        }
    };
}


using ::BearSSL::X509List;
#endif