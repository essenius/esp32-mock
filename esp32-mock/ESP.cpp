// Copyright 2021-2023 Rik Essenius
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
// ReSharper disable CppInconsistentNaming 
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppMemberFunctionMayBeConst
// ReSharper disable CppParameterMayBeConst
// ReSharper disable CppParameterNeverUsed

#include "ESP.h"
#include <chrono>
#include <cstdarg>

Esp ESP;

// Simulate changes as well as staying the same
int Esp::getFreeHeap() {
    _heapCount++;
    if (_heapCount <= 6) return 32000L - _heapCount * 3000L;
    if (_heapCount == 7) return 14000;
    _heapCount = -1;
    return 11000L;
}

// GPIO functions

constexpr uint8_t _espPinCount = 36;
uint8_t pinValue[_espPinCount];
uint8_t pinModeValue[_espPinCount];

uint8_t digitalRead(uint8_t pin) {
    return pinValue[pin];
}

void digitalWrite(uint8_t pin, uint8_t value) {
    pinValue[pin] = value;
}

uint8_t getPinMode(uint8_t pin) {
    return pinModeValue[pin];
}

void pinMode(uint8_t pin, uint8_t mode) {
    pinModeValue[pin] = mode;
}

// Time functions

unsigned long _espMicros = 0;
unsigned long _espMicrosSteps = 50;
bool _espRealTimeOn = false;
auto _espStartTime = std::chrono::high_resolution_clock::now();
bool _espDisableDelay = false;

long long _espMicroShift = 0;

void configTime(int i, int i1, const char* str, const char* text) {}

void shiftMicros(long long shift) {
    _espMicroShift = shift;
}

void delayMicroseconds(unsigned long delay) {
    if (_espRealTimeOn) {
        _espMicroShift += delay;
    }
    else {
        _espMicros += delay;
    }
}

void disableDelay(bool disable) {
    _espDisableDelay = disable;
}

void delay(int delay) {
    if (_espDisableDelay) return;
    if (_espRealTimeOn) {
        _espMicroShift += delay * 1000LL;
    }
    else {
        _espMicros += delay * 1000UL;
    }
}

void setRealTime(bool on) {
    _espRealTimeOn = on;
    if (_espRealTimeOn) {
        _espStartTime = std::chrono::high_resolution_clock::now();
    }
    else {
        _espMicros = 0;
    }
}

unsigned long millis() {
    return micros() / 1000;
}

unsigned long micros() {
    if (_espRealTimeOn) {
        const auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(now - _espStartTime).count() + _espMicroShift;
    }
    _espMicros += _espMicrosSteps;
    return _espMicros;
}

// Serial class

HardwareSerial Serial;

int HardwareSerial::available() {
    return static_cast<int>(strlen(_inputBufferPointer));
}

void HardwareSerial::begin(int speed) {
    clearInput();
    clearOutput();
}

void HardwareSerial::clearInput() {
    _inputBuffer[0] = 0;
    _inputBufferPointer = _inputBuffer;
}

void HardwareSerial::clearOutput() {
    _printBuffer[0] = 0;
    _printBufferPointer = _printBuffer;
}

const char* HardwareSerial::getOutput() {
    return _printBuffer;
}

void HardwareSerial::print(const char* input) {
    SafeCString::strcat(_printBuffer, input);
    _printBufferPointer = _printBuffer + strlen(_printBuffer);
}

void HardwareSerial::println(const char* input) {
    print(input);
    SafeCString::strcat(_printBuffer, "\n");
    _printBufferPointer = _printBuffer + strlen(_printBuffer);
}

char HardwareSerial::read() {
    if (*_inputBufferPointer == 0) return 0;
    return *_inputBufferPointer++;
}

void HardwareSerial::setInput(const char* input) {
    SafeCString::strcpy(_inputBuffer, input);
    _inputBufferPointer = _inputBuffer;
}

void HardwareSerial::setTimeout(long timeout) {}

const char* toString(LogLevel level) {
    switch (level) {
    case LogLevel::Error: return "E";
    case LogLevel::Warning: return "W";
    case LogLevel::Info: return "I";
    case LogLevel::Debug: return "D";
    case LogLevel::Verbose: return "V";
    }
    return nullptr;
}

int _espTimerAlarmEnabled = false;
hw_timer_t dummy{ 1, 1 };

hw_timer_t* timerBegin(uint8_t num, uint16_t divider, bool countUp) {
    dummy.num = num;
    dummy.group = static_cast<uint8_t>(divider);
    return &dummy;
}

void timerAlarmEnable(hw_timer_t* timer) {
    _espTimerAlarmEnabled = true;
}

void timerEnd(hw_timer_t* timer) {
    dummy.num = 255;
    dummy.group = 255;
}

LogLevel minLogLevel = LogLevel::Info;


