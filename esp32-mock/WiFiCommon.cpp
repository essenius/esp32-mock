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

// Mock implementation for unit testing (not targeting the ESP32)

// Disabling warnings caused by mimicking existing interface
// ReSharper disable CppInconsistentNaming
// ReSharper disable CppMemberFunctionMayBeConst
// ReSharper disable CppParameterMayBeConst

#include "WiFiCommon.h"
#include "StringArduino.h"


WiFiCommon::WiFiCommon() {
    testReset();
    for (byte i = 0; i < 6; i++) {
        _mac[i] = i * 17;
    }
}

String WiFiCommon::macAddress() {
    char buffer[20];
    SafeCString::sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", _mac[0], _mac[1], _mac[2], _mac[3], _mac[4], _mac[5]);
    return {buffer};
}

void WiFiCommon::testReset() {
    SafeCString::strcpy(_name, "esp32_001122334455");
    _ssid[0] = 0;
    _localIP = IPAddress(10, 0, 0, 2);
    _gatewayIP = IPAddress(10, 0, 0, 1);
    _subnetIP = IPAddress(255, 255, 0, 0);
    _primaryDNSIP = IPAddress(8, 8, 8, 8);
    _secondaryDNSIP = IPAddress(8, 8, 4, 4);
	_status = WL_DISCONNECTED;
    testConnectIn(kDefaultConnectDelay);
}

void WiFiCommon::testConnectIn(const int connectCount) {
    _connectMax = connectCount;
    _connectCountdown = _connectMax;
}

bool WiFiCommon::config(IPAddress local, IPAddress gateway, IPAddress subnet, IPAddress dns1, IPAddress dns2) {
    _localIP = local;
    _gatewayIP = gateway;
    _subnetIP = subnet;
    _primaryDNSIP = dns1 == INADDR_NONE ? _gatewayIP : dns1;
    _secondaryDNSIP = dns2 == INADDR_NONE ? _primaryDNSIP : dns2;
    return true;
}

bool WiFiCommon::isConnected() {
    if (_connectCountdown <= 0) return true;
    _connectCountdown--;
    return false;
}

bool WiFiCommon::setHostname(const char* name) {
    if(strlen(name) == 0) return false;
    SafeCString::strcpy(_name, name);
    return true;
}
