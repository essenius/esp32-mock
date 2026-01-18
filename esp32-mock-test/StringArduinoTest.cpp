// Copyright 2024-2026 Rik Essenius
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
#include "../esp32-mock/ESP.h"

namespace esp32_mock_test {
	TEST(StringArduinoTest, InitTest) {
		const String string1 = "11";
		EXPECT_TRUE(string1 == "11");
		EXPECT_TRUE(string1 != "hi");
		EXPECT_FALSE(string1 == "hi");
		EXPECT_FALSE(string1 != "11");

		EXPECT_EQ(string1.toInt(), 11);
		const auto convertedString = string1.c_str();
		EXPECT_STREQ(convertedString, "11");
		EXPECT_EQ(String("a").toInt(), 0);
	}
}
