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
#include "../esp32-mock/HTTPClient.h"

namespace esp32_mock_test {
	TEST(HttpClientTest, InitTest) {
		WiFiClient client;
		HTTPClient http;
		EXPECT_TRUE(http.begin(client, "http://www.example.com"));
		EXPECT_EQ(http.GET(), 400);
		EXPECT_STREQ(http.getString().c_str(), "0.1.1");
		http.end();
	}
}
