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
#include "../esp32-mock/IPAddress.h"

namespace esp32_mock_test {
	TEST(IPAddressTest, InitTest) {
		uint8_t bytes[4] = {1, 2, 3, 4};
		IPAddress ip(bytes);
		EXPECT_EQ(ip[0], 1);
		EXPECT_EQ(ip[1], 2);
		EXPECT_EQ(ip[2], 3);
		EXPECT_EQ(ip[3], 4);
		EXPECT_STREQ(ip.toString().c_str(), "1.2.3.4");
		IPAddress ip2(5, 6, 7, 8);
		EXPECT_FALSE(ip == ip2);
		ip2 = bytes;
		EXPECT_TRUE(ip == ip2);
		ip2 = 0x01234567;
		EXPECT_STREQ(ip2.toString().c_str(), "103.69.35.1");
		EXPECT_EQ(INADDR_NONE, 0);
	}
}
