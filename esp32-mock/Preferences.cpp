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

// Disabling warnings caused by mimicking existing interface
// ReSharper disable CppMemberFunctionMayBeConst
// ReSharper disable CppParameterMayBeConst

#include "Preferences.h"

#include <fstream>
#include <sstream>

bool Preferences::begin(const char* name, bool /*readOnly*/) {
    _started = true;
    _currentPreference = &_preferences[name];
    return true;
}

void Preferences::clear() {
    if (!_started) return;
    _currentPreference->clear();
}

void Preferences::end() {
    _started = false;
    _currentPreference = nullptr;
}

bool Preferences::getBool(const char* key, const bool defaultValue) {
    return getUInt(key, defaultValue);
}

void Preferences::getBytes(const char* key, void* buf, const size_t maxLen) {
    if (!isKey(key)) {
        buf = nullptr;
        return;
    }
    (*_currentPreference)[key].copy(static_cast<char*>(buf), maxLen);
}

String Preferences::getString(const char* key) {
    if (!_started) return "";
    return {(*_currentPreference)[key].c_str()};
}

unsigned Preferences::getUInt(const char* key, const int defaultValue) {
    if (!isKey(key)) return defaultValue;
    return std::stoi((*_currentPreference)[key]);
}

bool Preferences::isKey(const char* key) {
    if (!_started) return false;
    return _currentPreference->find(key) != _currentPreference->end();
}

void Preferences::putString(const char* key, const char* value) {
    if (!_started) return;
    (*_currentPreference)[key] = value;
}

void Preferences::putBool(const char* key, bool value) {
    putUInt(key, value);
}

void Preferences::putBytes(const char* key, const void* value, size_t len) {
    if (!_started) return;

    (*_currentPreference)[key] = std::string(len, 0);
    const auto p = static_cast<const char*>(value);
    (*_currentPreference)[key].assign(p, len);
}

void Preferences::putUInt(const char* key, uint32_t value) {
    if (!_started) return;
    (*_currentPreference)[key] = std::to_string(value);
}

void Preferences::save() {
    std::ofstream stream;
    stream.open("preferences.txt");
    for (const auto& categories : _preferences) {
        stream << categories.first << "\n";
        for (auto& entry : categories.second) {
            stream << entry.first << "=" << entry.second << "\n";
        }
        stream << "\n";
    }
    stream.close();
}

void Preferences::load() {
    std::ifstream stream;
    _preferences.clear();
    std::string categoryKey;
    stream.open("preferences.txt");
    std::string line;
    while (std::getline(stream, line)) {
        if (line.length() == 0) continue;
        const size_t equalsSign = line.find('=');
        if (equalsSign == std::string::npos) {
            categoryKey = line;
            continue;
        }
        std::string key = line.substr(0, equalsSign);
        std::string value = line.substr(equalsSign + 1);
        _preferences[categoryKey].insert(std::pair<std::string, std::string>(key, value));
    }
    stream.close();
}
