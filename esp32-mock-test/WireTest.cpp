// Copyright 2023-2024 Rik Essenius
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

namespace Esp32MockTest {
	TEST(WireTest, InitTest) {
		Wire.begin();
		Wire.beginTransmission(0x12);
		EXPECT_EQ(0x12, Wire.getAddress());
		EXPECT_EQ(1, Wire.write(0x34));
		EXPECT_EQ(1, Wire.write(0x56));
		EXPECT_EQ(1, Wire.write(0x78));
		EXPECT_EQ(1, Wire.write(0x9A));
		EXPECT_EQ(1, Wire.write(0xBC));
		EXPECT_EQ(1, Wire.write(0xDE));
		EXPECT_EQ(1, Wire.write(0xF0));
		EXPECT_EQ(1, Wire.write(0x12));

		uint8_t buffer[] = { 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12 };
		EXPECT_EQ(8, Wire.writeMismatchIndex(buffer, sizeof(buffer)));
		buffer[4] = 0xff;
		EXPECT_EQ(4, Wire.writeMismatchIndex(buffer, sizeof(buffer)));
		EXPECT_EQ(-1, Wire.writeMismatchIndex({}, 0));

		Wire.endTransmission();

		EXPECT_EQ(1, Wire.available());
		EXPECT_EQ(2, Wire.available());
		EXPECT_EQ(3, Wire.available());

		EXPECT_EQ(0, Wire.requestFrom(0x12, 1, true));
		EXPECT_EQ(0, Wire.read());
		EXPECT_EQ(1, Wire.read());
		EXPECT_EQ(2, Wire.read());

		Wire.setFlatline(true, 33);
		EXPECT_EQ(33, Wire.read());
		EXPECT_EQ(33, Wire.read());
		EXPECT_EQ(33, Wire.read());

		Wire.setFlatline(false);
		EXPECT_EQ(0, Wire.read());
		EXPECT_EQ(1, Wire.read());
		EXPECT_EQ(2, Wire.read());

		EXPECT_EQ(-5, Wire.available());

		Wire.setEndTransmissionTogglePeriod(0);
		EXPECT_EQ(0, Wire.endTransmission());

		Wire.setEndTransmissionTogglePeriod(2);
		EXPECT_EQ(0, Wire.endTransmission());
		EXPECT_EQ(0, Wire.endTransmission());
		EXPECT_EQ(1, Wire.endTransmission());
	}

	TEST(WireTest, OvrerrunTest) {
		Wire.begin();
		Wire.beginTransmission(0x12);
		for (int i = 0; i < 2048; i++) {
			EXPECT_EQ(1, Wire.write(i % 256));
		}
		EXPECT_EQ(0, Wire.write(65));
	}

}