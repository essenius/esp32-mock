
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

// Disabling warnings caused by mimicking existing interface
// ReSharper disable CppMemberFunctionMayBeConst
// ReSharper disable CppParameterMayBeConst

#include "Preferences.h"

#include <fstream>
#include <sstream>
#include <algorithm>

bool Preferences::begin(const char* name, bool /*readOnly*/) {
    _started = true;
    load();
    _currentPreference = &_preferences[name];
    return true;
}

void Preferences::clear() {
    if (!_started) return;
    _currentPreference->clear();
}

void Preferences::end() {
    save();
    _started = false;
    _currentPreference = nullptr;
}

bool Preferences::getBool(const char* key, const bool defaultValue) {
    return getUInt(key, defaultValue);
}

void Preferences::getBytes(const char* key, void* buf, const size_t maxLen) {
    if (!isKey(key)) {
        static_cast<char *>(buf)[0] = '\0';
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

void Preferences::testReset() {
    (void)std::remove("preferences.txt");
}

void Preferences::save() {
    std::ofstream stream;
    stream.open("preferences.txt");
    for (const auto& categories : _preferences) {
        stream << categories.first << "\n";
        for (auto& entry : categories.second) {
            stream << entry.first << "=";
            size_t newlineCount = std::count(entry.second.begin(), entry.second.end(), '\n');
            if (!entry.second.empty() && entry.second.back() != '\n') {
                newlineCount++;
            }
            if (newlineCount > 1 || entry.second[0] == '#') {
                stream << "#" << newlineCount << "\n";
            }
            stream << entry.second << "\n";
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

        if (value[0] == '#') {
            const auto numberOfLines = std::stoi(value.substr(1));
            value = "";

            for (int i = 0; i < numberOfLines; i++) {
                std::string valueLine;
                if (!std::getline(stream, valueLine)) break;
                value += valueLine + (numberOfLines > 1 ? "\n" : "");
            }
        }
        _preferences[categoryKey].insert(std::pair<std::string, std::string>(key, value));
    }
    stream.close();
}

