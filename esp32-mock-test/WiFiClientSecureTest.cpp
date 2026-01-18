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
#include "../esp32-mock/WiFiClientSecure.h"

namespace esp32_mock_test {
	TEST(WiFiClientSecureTest, InitTest) {
		WiFiClientSecure client1;
		EXPECT_STREQ(client1.testGetType(), "WifiClientSecure");
		EXPECT_TRUE(client1.testIsSecure());
		client1.setInsecure();
		EXPECT_FALSE(client1.testIsSecure());

		WiFiClient client2;
		EXPECT_STREQ(client2.testGetType(), "WifiClient");
		EXPECT_TRUE(client2.isConnected());
		client2.stop();

		Client client3;
		EXPECT_STREQ(client3.testGetType(), "Client");
	}
}
