// Copyright 2023-2026 Rik Essenius
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
// except in compliance with the License. You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and limitations under the License.

#include <gtest/gtest.h>
#include "../esp32-mock/ESP8266WiFi.h"
#include "../esp32-mock/WiFiClientSecure.h"
#include "../esp32-mock/ESP8266httpUpdate.h"

namespace esp8266_mock_test {

	namespace {
		int progress_count = 0;
		bool started = false;
		bool finished = false;
	}

	TEST(ESP8266httpUpdateTest, InitTest) {
		EXPECT_EQ(0, ESPhttpUpdate.getLastError());
		EXPECT_STREQ("OK", ESPhttpUpdate.getLastErrorString().c_str());
		WiFiClient client;
		EXPECT_EQ(HTTP_UPDATE_NO_UPDATES, ESPhttpUpdate.update(client, nullptr));
		EXPECT_EQ(0, progress_count);
		EXPECT_FALSE(started);
		EXPECT_FALSE(finished);
		HTTPUpdate::ReturnValue = HTTP_UPDATE_OK;
		ESPhttpUpdate.onProgress([](const int current, const int /*total*/) {
			progress_count++;

			if (progress_count == 1) {
				EXPECT_EQ(300000, current);
			}
			else {
				EXPECT_EQ(1000000, current);

			}
		});

		ESPhttpUpdate.onStart([] {
			started = true;
		});

		ESPhttpUpdate.onEnd([] {
			finished = true;
		});

		EXPECT_EQ(HTTP_UPDATE_OK, ESPhttpUpdate.update(client, nullptr));

		EXPECT_EQ(2, progress_count);
		EXPECT_TRUE(started);
		EXPECT_TRUE(finished);

		// reset for other tests
		HTTPUpdate::ReturnValue = HTTP_UPDATE_NO_UPDATES;
	}
}