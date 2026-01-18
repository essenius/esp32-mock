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

// Disabling warnings caused by mimicking existing interface
// ReSharper disable CppMemberFunctionMayBeConst
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppInconsistentNaming
// ReSharper disable CppParameterMayBeConst

#ifndef HEADER_WIFI_COMMON
#define HEADER_WIFI_COMMON

#include "ESP.h"
#include "IPAddress.h"
#include "StringArduino.h"

enum WiFiMode: uint8_t {
    WIFI_OFF = 0,
    WIFI_STA = 1,
    WIFI_AP = 2,
    WIFI_AP_STA = 3
};

enum wl_status_t: uint8_t {
  WL_NO_SHIELD = 255,  // for compatibility with Wi-Fi Shield library
  WL_STOPPED = 254,
  WL_IDLE_STATUS = 0,
  WL_NO_SSID_AVAIL = 1,
  WL_SCAN_COMPLETED = 2,
  WL_CONNECTED = 3,
  WL_CONNECT_FAILED = 4,
  WL_CONNECTION_LOST = 5,
  WL_DISCONNECTED = 6
};

/**
 * \brief Mock implementation of the WiFi class/object for unit testing (not targeting the ESP32)
 */
class WiFiCommon {
public:
    WiFiCommon();

    void mode(int m) { _mode = m; }
int mode() const { return _mode; }

    void begin(const char* ssid, const char* /*password*/, int /*ignore*/ = 0, const uint8_t* /*_bssid*/ = nullptr) {
        SafeCString::strcpy(_ssid, ssid);
    }

    bool config(IPAddress local, IPAddress gateway, IPAddress subnet,
                IPAddress dns1 = IPAddress(), IPAddress dns2 = IPAddress());
    String BSSIDstr() { return {"55:44:33:22:11:00"}; }
    int channel() { return 13; }
    void disconnect() { _connectCountdown = _connectMax; }
    IPAddress dnsIP(int i = 0) { return i == 0 ? _primaryDNSIP : _secondaryDNSIP; }
    IPAddress gatewayIP() { return _gatewayIP; }
    const char* getHostname() { return _name; }
    bool isConnected();
    IPAddress localIP() { return _localIP; }
    String macAddress();
    void macAddress(uint8_t* mac) { memcpy(mac, _mac, 6); }
    IPAddress networkID() { return {192, 168, 1, 0}; }
    void reconnect() { _connectCountdown = _reconnectMax; }
    int RSSI() { return 1; }
    bool setHostname(const char* name);
    String SSID() { return {_ssid}; }
    wl_status_t status() { return _status; }
    IPAddress subnetMask() const { return _subnetIP; }

	// testing functions, not part of the real interface
    void testConnectIn(int connectCount);
    void testSetStatus(wl_status_t status) { _status = status; }
    void testReset();
private:
    static constexpr int kDefaultConnectDelay = 5;
    byte _mac[6]{};
    char _ssid[20] = {};
    int _mode = WIFI_STA;
    wl_status_t _status = WL_CONNECTED;
    IPAddress _localIP;
    IPAddress _gatewayIP;
    IPAddress _subnetIP;
    IPAddress _primaryDNSIP;
    IPAddress _secondaryDNSIP;
    int _connectMax = kDefaultConnectDelay;
    int _reconnectMax = 1;
    int _connectCountdown = _connectMax;
protected:
    char _name[25] = {};
};

#endif
