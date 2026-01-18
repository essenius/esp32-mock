// Copyright 2021-2026 Rik Essenius
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

// Disabling warnings caused by mimicking existing interfaces
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppMemberFunctionMayBeConst 
// ReSharper disable CppParameterMayBeConst
// ReSharper disable CppClangTidyPerformanceUnnecessaryValueParam

#include <SafeCString.h>
#include <algorithm>
#include "PubSubClient.h"

bool PubSubClient::connect(const char* id, const char* /*willTopic*/, uint8_t /*willQos*/, bool /*willRetain*/, const char* /*willMessage*/) {
    SafeCString::strcpy(_id, id);
    _connectCalled = _canConnect;
    return _canConnect;
}

bool PubSubClient::connect(const char* id, const char* user, const char* pass,
                           const char* willTopic, uint8_t willQos, bool willRetain, const char* willMessage) {
    SafeCString::strcpy(_user, user);
    SafeCString::strcpy(_pass, pass);
    return connect(id, willTopic, willQos, willRetain, willMessage);
}

void PubSubClient::setLoopCallback(const char* topic, const uint8_t* payload, int size) {
    SafeCString::strcpy(_loopTopic, topic);
    const auto usedSize = std::min(size, kSinglePayloadSize - 1);
    memcpy(_loopPayload, payload, usedSize);
    _loopPayloadSize = usedSize;
}

bool PubSubClient::loop() {
        _loopCount++;
        if (_loopTopic[0] != 0) {
            _callback(_loopTopic, _loopPayload, _loopPayloadSize);
        }
        return true;
}

bool PubSubClient::publish(const char* topic, const char* payload, bool retain) {
    if (strlen(topic) + strlen(_topic) > kTopicSize - 1) return false;
    SafeCString::strcat(_topic, topic);
    SafeCString::strcat(_topic, "\n");
    if (strlen(payload) + strlen(_payload) > kPayloadSize - 1) return false;
    SafeCString::strcat(_payload, payload);
    if (!retain) SafeCString::strcat(_payload, "[x]");
    SafeCString::strcat(_payload, "\n");
    _callCount++;
    return _canPublish && _callCount <= _callCountThreshold;
}

void PubSubClient::testResetHistory() {
    _callCount = 0;
    _payload[0] = 0;
    _topic[0] = 0; 
    
    _loopCount = 0;
    _loopTopic[0] = 0;
    _loopPayloadSize = 0;
}


void PubSubClient::testReset()
{
    _user[0] = 0;
    _pass[0] = 0;
    _id[0] = 0;
    _canConnect = true;
    _canSubscribe = true;
    _canPublish = true;
    _connectCalled = false;
    _callCountThreshold = kDefaultCallCountThreshold;

    testResetHistory();
}

PubSubClient& PubSubClient::setCallback(MQTT_CALLBACK_SIGNATURE) {
    _callback = callback;
    return *this;
}
