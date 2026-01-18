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
#include "../esp32-mock/Adafruit_SSD1306.h"

namespace esp32_mock_test {
	TEST(AdaFruitSsd1306TestTest, InitTest) {

		Adafruit_SSD1306 display(128, 64, nullptr);
		EXPECT_TRUE(display.begin(SSD1306_SWITCHCAPVCC, 0x3C));

		constexpr uint8_t bitmap[1] = { 10 };
		display.drawBitmap(1, 2, bitmap, 3, 4, 5, 6);
		EXPECT_EQ(display.getX(), 1);
		EXPECT_EQ(display.getY(), 2);
		EXPECT_EQ(display.getWidth(), 3);
		EXPECT_EQ(display.getHeight(), 4);
		EXPECT_EQ(display.getForegroundColor(), 5);
		EXPECT_EQ(display.getBackgroundColor(), 6);
		EXPECT_EQ(display.getSize(), 0);
		EXPECT_EQ(display.getFirstByte(), 10);
		EXPECT_STREQ(display.getMessage(), "");

		display.fillRect(11, 12, 13, 14, 15);
		EXPECT_EQ(display.getX(), 11);
		EXPECT_EQ(display.getY(), 12);
		EXPECT_EQ(display.getWidth(), 13);
		EXPECT_EQ(display.getHeight(), 14);
		EXPECT_EQ(display.getForegroundColor(), 15);
		EXPECT_EQ(display.getBackgroundColor(), 6);
		EXPECT_EQ(display.getSize(), 0);
		EXPECT_EQ(display.getFirstByte(), 10);
		EXPECT_STREQ(display.getMessage(), "");

		display.setCursor(21, 22);
		EXPECT_EQ(display.getX(), 21);
		EXPECT_EQ(display.getY(), 22);
		EXPECT_EQ(display.getWidth(), 13);

		display.setTextSize(31);
		EXPECT_EQ(display.getSize(), 31);
		EXPECT_EQ(display.getX(), 21);
		EXPECT_EQ(display.getY(), 22);

		display.setTextColor(41, 42);
		EXPECT_EQ(display.getForegroundColor(), 41);
		EXPECT_EQ(display.getBackgroundColor(), 42);
		EXPECT_EQ(display.getSize(), 31);
		EXPECT_EQ(display.getWidth(), 13);

		display.print("Hello");
		EXPECT_STREQ(display.getMessage(), "Hello");
		EXPECT_EQ(display.getHeight(), 14);
	}
}