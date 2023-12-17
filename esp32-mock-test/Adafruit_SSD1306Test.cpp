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
#include "../esp32-mock/Adafruit_SSD1306.h"

namespace Esp32MockTest {
	TEST(AdaFruitSsd1306TestTest, InitTest) {

		Adafruit_SSD1306 display(128, 64, nullptr);
		EXPECT_TRUE(display.begin(SSD1306_SWITCHCAPVCC, 0x3C));

		constexpr uint8_t Bitmap[1] = { 10 };
		display.drawBitmap(1, 2, Bitmap, 3, 4, 5, 6);
		EXPECT_EQ(1, display.getX());
		EXPECT_EQ(2, display.getY());
		EXPECT_EQ(3, display.getWidth());
		EXPECT_EQ(4, display.getHeight());
		EXPECT_EQ(5, display.getForegroundColor());
		EXPECT_EQ(6, display.getBackgroundColor());
		EXPECT_EQ(0, display.getSize());
		EXPECT_EQ(10, display.getFirstByte());
		EXPECT_STREQ("", display.getMessage());

		display.fillRect(11, 12, 13, 14, 15);
		EXPECT_EQ(11, display.getX());
		EXPECT_EQ(12, display.getY());
		EXPECT_EQ(13, display.getWidth());
		EXPECT_EQ(14, display.getHeight());
		EXPECT_EQ(15, display.getForegroundColor());
		EXPECT_EQ(6, display.getBackgroundColor());
		EXPECT_EQ(0, display.getSize());
		EXPECT_EQ(10, display.getFirstByte());
		EXPECT_STREQ("", display.getMessage());

		display.setCursor(21, 22);
		EXPECT_EQ(21, display.getX());
		EXPECT_EQ(22, display.getY());
		EXPECT_EQ(13, display.getWidth());

		display.setTextSize(31);
		EXPECT_EQ(31, display.getSize());
		EXPECT_EQ(21, display.getX());
		EXPECT_EQ(22, display.getY());

		display.setTextColor(41, 42);
		EXPECT_EQ(41, display.getForegroundColor());
		EXPECT_EQ(42, display.getBackgroundColor());
		EXPECT_EQ(31, display.getSize());
		EXPECT_EQ(13, display.getWidth());

		display.print("Hello");
		EXPECT_STREQ("Hello", display.getMessage());
		EXPECT_EQ(14, display.getHeight());
	}
}