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

// Mock implementation for unit testing (not targeting the ESP32)

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include "time.h"  // NOLINT(modernize-deprecated-headers)
#include <Windows.h>

int gettimeofday(timeval* timeVal, void* /*ignore*/) {
    if (timeVal) {
        // 0.1 microsecond intervals since January 1, 1601 00:00 UTC 
        FILETIME fileTime{ 0,0 }; 

#if _WIN32_WINNT >= _WIN32_WINNT_WIN8
        GetSystemTimePreciseAsFileTime(&fileTime);
#else
        GetSystemTimeAsFileTime(&filetime);
#endif
        const ULARGE_INTEGER x {{fileTime.dwLowDateTime, fileTime.dwHighDateTime}};

    	// microseconds between Jan 1,1601 and Jan 1,1970 (start date of Unix epoch)
        static constexpr ULONGLONG EpochOffsetMicroSeconds = 11644473600000000ULL;

    	const ULONGLONG microSeconds = x.QuadPart / 10 - EpochOffsetMicroSeconds;
        timeVal->tv_sec = static_cast<time_t>(microSeconds / 1000000ULL);
        timeVal->tv_usec = static_cast<long>(microSeconds % 1000000ULL);
    }
    return 0;
}
#endif