// Copyright 2023 Rik Essenius
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

namespace Esp32MockTest {
	static int progressCount = 0;
	static bool started = false;
	static bool finished = false;

	TEST(HttpUpdateTest, InitTest) {
		EXPECT_EQ(0, httpUpdate.getLastError());
		EXPECT_STREQ("OK", httpUpdate.getLastErrorString().c_str());
		WiFiClient client;
		EXPECT_EQ(HTTP_UPDATE_NO_UPDATES, httpUpdate.update(client, nullptr));
		EXPECT_EQ(0, progressCount);
		EXPECT_FALSE(started);
		EXPECT_FALSE(finished);
		httpUpdate.ReturnValue = HTTP_UPDATE_OK;
		httpUpdate.onProgress([=](const int current, const int /*total*/) {
			progressCount++;

			if (progressCount == 1) {
				EXPECT_EQ(300000, current);
			}
			else {
				EXPECT_EQ(1000000, current);

			}
			});

		httpUpdate.onStart([] {
			started = true;
			});

		httpUpdate.onEnd([] {
			finished = true;
			});

		EXPECT_EQ(HTTP_UPDATE_OK, httpUpdate.update(client, nullptr));

		EXPECT_EQ(2, progressCount);
		EXPECT_TRUE(started);
		EXPECT_TRUE(finished);
	}
}