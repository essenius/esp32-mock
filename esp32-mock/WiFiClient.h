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

// Disabling warnings caused by mimicking existing interfaces
// ReSharper disable CppMemberFunctionMayBeStatic

#ifndef HEADER_WIFI_CLIENT
#define HEADER_WIFI_CLIENT

#include "Client.h"

/**
 * \brief Mock implementation of WiFiClient for unit testing (not targeting the ESP32)
 */
class WiFiClient : public Client {
public:
    WiFiClient() = default;
    bool isConnected() { return true; }
    void stop() {}

    //testing
    const char* getType() override { return "WifiClient"; }
};

#endif
