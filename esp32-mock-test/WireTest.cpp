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
#include "../esp32-mock/Wire.h"

namespace esp32_mock_test {
	TEST(WireTest, InitTest) {
		Wire.begin();
		Wire.beginTransmission(0x12);
		EXPECT_EQ(Wire.getAddress(), 0x12);
		EXPECT_EQ(Wire.write(0x34), 1);
		EXPECT_EQ(Wire.write(0x56), 1);
		EXPECT_EQ(Wire.write(0x78), 1);
		EXPECT_EQ(Wire.write(0x9A), 1);
		EXPECT_EQ(Wire.write(0xBC), 1);
		EXPECT_EQ(Wire.write(0xDE), 1);
		EXPECT_EQ(Wire.write(0xF0), 1);
		EXPECT_EQ(Wire.write(0x12), 1);

		uint8_t buffer[] = { 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12 };
		EXPECT_EQ(Wire.testWriteMismatchIndex(buffer, sizeof(buffer)), 8);
		buffer[4] = 0xff;
		EXPECT_EQ(Wire.testWriteMismatchIndex(buffer, sizeof(buffer)), 4);
		EXPECT_EQ(Wire.testWriteMismatchIndex({}, 0), -1);

		Wire.endTransmission();

		EXPECT_EQ(Wire.available(), 1);
		EXPECT_EQ(Wire.available(), 2);
		EXPECT_EQ(Wire.available(), 3);

		EXPECT_EQ(Wire.requestFrom(0x12, 1, true), 0);
		EXPECT_EQ(Wire.read(), 0);
		EXPECT_EQ(Wire.read(), 1);
		EXPECT_EQ(Wire.read(), 2);

		Wire.setFlatline(true, 33);
		EXPECT_EQ(Wire.read(), 33);
		EXPECT_EQ(Wire.read(), 33);
		EXPECT_EQ(Wire.read(), 33);

		Wire.setFlatline(false);
		EXPECT_EQ(Wire.read(), 0);
		EXPECT_EQ(Wire.read(), 1);
		EXPECT_EQ(Wire.read(), 2);

		EXPECT_EQ(Wire.available(), -5);

		Wire.setEndTransmissionTogglePeriod(0);
		EXPECT_EQ(Wire.endTransmission(), 0);

		Wire.setEndTransmissionTogglePeriod(2);
		EXPECT_EQ(Wire.endTransmission(), 0);
		EXPECT_EQ(Wire.endTransmission(), 0);
		EXPECT_EQ(Wire.endTransmission(), 1);
	}

	TEST(WireTest, OverrunTest) {
		Wire.begin();
		Wire.beginTransmission(0x12);
		for (uint16_t i = 0; i < 2048; i++) {
			EXPECT_EQ(Wire.write(static_cast<uint8_t>(i % 256)), 1);
		}
		EXPECT_EQ(Wire.write(65), 0);
	}

}