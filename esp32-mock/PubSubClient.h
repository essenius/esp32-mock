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

// Disabling warnings caused by mimicking existing interfaces
// ReSharper disable CppMemberFunctionMayBeStatic 
// ReSharper disable CppMemberFunctionMayBeConst
// ReSharper disable CppParameterMayBeConst

#ifndef HEADER_PUBSUBCLIENT
#define HEADER_PUBSUBCLIENT

#include <functional>
#include <cstdint>
#include "Client.h"

#define MQTT_CALLBACK_SIGNATURE std::function<void(char*, uint8_t*, unsigned int)> callback

/**
 * \brief Mock implementation of the PubSubClient library for unit testing (not targeting the ESP32)
 */
class PubSubClient {
public:
    bool connect(const char* id, const char* willTopic, uint8_t willQos, bool willRetain, const char* willMessage);
    bool connect(const char* id, const char* user, const char* pass, const char* willTopic, uint8_t willQos, bool willRetain,
                 const char* willMessage);
    bool connected() { return _connectCalled && _canConnect; }
    void disconnect() { _connectCalled = false; }
    void setLoopCallback(const char* topic, const uint8_t* payload, int size);

    bool loop();
    bool publish(const char* topic, const char* payload, bool retain = false);
    bool setBufferSize(int /*size*/) { return true; }
    PubSubClient& setCallback(MQTT_CALLBACK_SIGNATURE);
    PubSubClient& setClient(Client& /*client*/) { return *this; }
    PubSubClient& setKeepAlive(uint16_t /*period*/) { return *this; }
    PubSubClient& setServer(const char* /*broker*/, uint16_t /*port*/) { return *this; }
    bool subscribe(const char* /*topic*/) { return _canSubscribe; }
    bool unsubscribe(const char* /*topic*/) { return true; }
    int state() { return 3; }

    // test assistance functions
    void testCallBack(char* topic, uint8_t* payload, unsigned int size) { _callback(topic, payload, size); }
    void testFailPublishAfter(int callCount) { _callCountThreshold = callCount; }
    int testGetCallCount() const { return _callCount; }
    int testGetLoopCount() const { return _loopCount; }
    const char* testGetPayloads() const { return _payload; }
    const char* testGetTopics() const { return _topic; }
    const char* testGetId() const { return _id; }
    void testReset();
    void testResetHistory();
    void testSetCanConnect(bool canConnect) { _canConnect = canConnect; }
    void testSetCanPublish(bool canPublish) { _canPublish = canPublish; }
    void testSetCanSubscribe(bool canSubscribe) { _canSubscribe = canSubscribe; }
    const char* testGetUser() const { return _user; }

private:
    constexpr static size_t kFieldSize = 64;
    constexpr static size_t kPayloadSize = 1024;
    constexpr static size_t kTopicSize = 2500;
    constexpr static size_t kSingleTopicSize = 100;
    constexpr static int kSinglePayloadSize = 20;
    constexpr static size_t kDefaultCallCountThreshold = 1000000;
    std::function<void(char*, unsigned char*, unsigned)> _callback;
    int _callCount = 0;
    int _callCountThreshold = kDefaultCallCountThreshold;
    bool _canConnect = true;
    bool _canPublish = true;
    bool _canSubscribe = true;
    bool _connectCalled = false;
    char _id[kFieldSize] = {};
    int _loopCount = 0;
    char _pass[kFieldSize] = {};
    char _payload[kPayloadSize] = {};
    char _topic[kTopicSize] = {};
    char _user[kFieldSize] = {};
    char _loopTopic[kSingleTopicSize] = "\0";
    uint8_t _loopPayload[kSinglePayloadSize] = {};
    int _loopPayloadSize = 0;
};

#endif
