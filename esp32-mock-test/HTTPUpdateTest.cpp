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
#include "../esp32-mock/HTTPUpdate.h"

namespace esp32_mock_test {
	namespace {
		int progress_count = 0;
		bool started = false;
		bool finished = false;
	}

	TEST(HttpUpdateTest, InitTest) {
		EXPECT_EQ(httpUpdate.getLastError(), 0);
		EXPECT_STREQ(httpUpdate.getLastErrorString().c_str(), "OK");
		WiFiClient client;
		httpUpdate.setLedPin(2, HIGH);
		EXPECT_EQ(httpUpdate.testGetLedPin(), 2);
		EXPECT_EQ(httpUpdate.testGetLedOn(), HIGH);
		EXPECT_EQ(httpUpdate.update(client, nullptr), HTTP_UPDATE_NO_UPDATES);
		EXPECT_EQ(progress_count, 0);
		EXPECT_FALSE(started);
		EXPECT_FALSE(finished);
		HTTPUpdate::ReturnValue = HTTP_UPDATE_OK;
		httpUpdate.onProgress([](const int current, const int /*total*/) {
			progress_count++;

			if (progress_count == 1) {
				EXPECT_EQ(current, 300000);
			}
			else {
				EXPECT_EQ(current, 1000000);
			}
		});

		httpUpdate.onStart([] {
			started = true;
		});

		httpUpdate.onEnd([] {
			finished = true;
		});

		EXPECT_EQ(httpUpdate.update(client, nullptr), HTTP_UPDATE_OK);

		EXPECT_EQ(progress_count, 2);
		EXPECT_TRUE(started);
		EXPECT_TRUE(finished);
	}
}
