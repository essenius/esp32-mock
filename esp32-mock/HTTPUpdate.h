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
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppInconsistentNaming
// ReSharper disable CppParameterMayBeConst
// ReSharper disable CppClangTidyPerformanceUnnecessaryValueParam
// ReSharper disable CppPassValueParameterByConstReference

#ifndef HEADER_HTTPUPDATE
#define HEADER_HTTPUPDATE

#include <functional>

#include "StringArduino.h"
#include "WiFiClient.h"
#include "ESP.h"

enum HTTPUpdateResult: uint8_t {
  HTTP_UPDATE_FAILED,
  HTTP_UPDATE_NO_UPDATES,
  HTTP_UPDATE_OK
};

typedef HTTPUpdateResult t_httpUpdate_return; // backward compatibility

using HTTPUpdateStartCB = std::function<void()>;
using HTTPUpdateEndCB = std::function<void()>;
using HTTPUpdateErrorCB = std::function<void(int)>;
using HTTPUpdateProgressCB = std::function<void(int, int)>;

/**
 * \brief Mock implementation of HTTPUpdate for unit testing (not targeting the ESP32)
 */
class HTTPUpdate {
public:
    t_httpUpdate_return update(WiFiClient& client, const char* url);

    int getLastError() { return 0; }
    String getLastErrorString() { return {"OK"}; }
    static t_httpUpdate_return ReturnValue;

    void onStart(HTTPUpdateStartCB cbOnStart) { _cbStart = cbOnStart; }
    void onEnd(HTTPUpdateEndCB cbOnEnd) { _cbEnd = cbOnEnd; }
    void onError(HTTPUpdateErrorCB cbOnError) { _cbError = cbOnError; }
    void onProgress(HTTPUpdateProgressCB cbOnProgress) { _cbProgress = cbOnProgress; }

    void setLedPin(int ledPin, uint8_t ledOn);

    // testing only
    const char* testGetUrl() const { return _url; }
	int testGetLedPin() const { return _ledPin; }
	int testGetLedOn() const { return _ledOn; }

private:
    HTTPUpdateStartCB    _cbStart;
    HTTPUpdateEndCB      _cbEnd;
    HTTPUpdateErrorCB    _cbError;
    HTTPUpdateProgressCB _cbProgress;

    int _ledPin = -1;
    uint8_t _ledOn = LOW;
    const char* _url = nullptr;
};

extern HTTPUpdate httpUpdate;

#endif
