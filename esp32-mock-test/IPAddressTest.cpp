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
#include "../esp32-mock/IPAddress.h"

namespace Esp32MockTest {
	TEST(IPAddressTest, InitTest) {
		uint8_t bytes[4] = { 1, 2, 3, 4 };
		IPAddress ip(bytes);
		EXPECT_EQ(1, ip[0]);
		EXPECT_EQ(2, ip[1]);
		EXPECT_EQ(3, ip[2]);
		EXPECT_EQ(4, ip[3]);
		EXPECT_STREQ("1.2.3.4", ip.toString().c_str());
		IPAddress ip2(5, 6, 7, 8);
		EXPECT_FALSE(ip == ip2);
		ip2	= bytes;
		EXPECT_TRUE(ip == ip2);
		ip2 = 0x01234567;
		EXPECT_STREQ("103.69.35.1", ip2.toString().c_str());
		EXPECT_EQ(0, INADDR_NONE);
	}
}