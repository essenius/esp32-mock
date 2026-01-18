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
#include "../esp32-mock/ESP.h"

namespace esp32_mock_test {
	TEST(ESPTest, InitTest) {
		EXPECT_DOUBLE_EQ(3.1415926535897932384626433832795, PI);

		ESP.restart();
		EXPECT_EQ(ESP.getFreeHeap(), 32000L) << "First call to getFreeHeap returns initial value";
		EXPECT_EQ(ESP.getFreeHeap(), 29000L) << "Second call to getFreeHeap decreases";
		EXPECT_EQ(ESP.getFreeHeap(), 26000L) << "Third call to getFreeHeap decreases";
		EXPECT_EQ(ESP.getFreeHeap(), 23000L) << "4th call to getFreeHeap decreases";
		EXPECT_EQ(ESP.getFreeHeap(), 20000L) << "5th call to getFreeHeap decreases";
		EXPECT_EQ(ESP.getFreeHeap(), 17000L) << "6th call to getFreeHeap decreases";
		EXPECT_EQ(ESP.getFreeHeap(), 14000L) << "7th call to getFreeHeap decreases";
		EXPECT_EQ(ESP.getFreeHeap(), 14000L) << "8th call to getFreeHeap stays equal";
		EXPECT_EQ(ESP.getFreeHeap(), 11000L) << "9th call to getFreeHeap decreases";
		EXPECT_EQ(ESP.getFreeHeap(), 32000L) << "10th call to getFreeHeap resets to initial value";
	}

	TEST(ESPTest, DigitalReadWriteTest) {
		EXPECT_EQ(digitalRead(0), LOW) << "Initial digital read is LOW";
		digitalWrite(0, HIGH);
		EXPECT_EQ(digitalRead(0), HIGH) << "Read written value";
		EXPECT_EQ(testGetPinMode(0), INPUT) << "Initial pin mode in INPUT";
		pinMode(0, OUTPUT);
		EXPECT_EQ(testGetPinMode(0), OUTPUT) << "Pin mode changed to OUTPUT";
	}

	TEST(ESPTest, realtimeOfftimeTest) {
		testSetRealTime(false);
		configTime(0, 0, nullptr, nullptr);
		EXPECT_EQ(micros(), 50);
		EXPECT_EQ(micros(), 100);
		delayMicroseconds(1000);
		EXPECT_EQ(micros(), 1150);
		EXPECT_EQ(millis(), 1);
		delay(1);
		EXPECT_EQ(millis(), 2);
	}

	TEST(ESPTest, realtimeOntimeTest) {
		testSetRealTime(true);
		EXPECT_EQ(millis(), 0);
		delay(5);
		auto currentTime = micros();
		EXPECT_TRUE(currentTime >= 5000L && currentTime < 5200);
		testDisableDelay(true);
		delay(5);
		currentTime = micros();
		EXPECT_TRUE(currentTime >= 5000L && currentTime < 5200);
		testDisableDelay(false);
		delay(5);
		currentTime = micros();
		EXPECT_TRUE(currentTime >= 10000L && currentTime < 10200);
		currentTime = micros();
		delayMicroseconds(100);
		EXPECT_TRUE(micros() >= currentTime + 100);
		testShiftMicros(200);
		currentTime = micros();
		EXPECT_TRUE(currentTime >= 200 && currentTime < 300);
		testSetRealTime(false);
	}

	TEST(ESPTest, SerialTest) {
		Serial.begin(115200);
		Serial.setTimeout(1000);
		EXPECT_STREQ(Serial.testGetOutput(), "");
		Serial.print("Hello");
		EXPECT_STREQ(Serial.testGetOutput(), "Hello");
		Serial.println(" There");
		EXPECT_STREQ(Serial.testGetOutput(), "Hello There\n");
		EXPECT_EQ(Serial.available(), 0);
		EXPECT_EQ(Serial.read(), 0);
		Serial.testSetInput("Hello");
		EXPECT_EQ(Serial.read(), 'H');
		EXPECT_EQ(Serial.read(), 'e');
		EXPECT_EQ(Serial.available(), 3);
	}

	TEST(ESPTest, LogLevelTest) {
		EXPECT_STREQ(toString(LogLevel::Error), "E");
		EXPECT_STREQ(toString(LogLevel::Warning), "W");
		EXPECT_STREQ(toString(LogLevel::Info), "I");
		EXPECT_STREQ(toString(LogLevel::Debug), "D");
		EXPECT_STREQ(toString(LogLevel::Verbose), "V");
		EXPECT_EQ(toString(static_cast<LogLevel>(6)), nullptr);
	}

	TEST(ESPTest, TimerTest) {
		yield(); // just make sure we can run it
		const auto result = timerBegin(1, 2, true);
		EXPECT_EQ(result->num, 1);
		EXPECT_EQ(result->group, 2);
		timerEnd(result);
		EXPECT_EQ(result->num, 255);
		EXPECT_EQ(result->group, 255);
	}

	TEST(ESPTest, xxx) {
		EXPECT_FALSE(testIsTimerAlarmEnabled());
		timerAlarmEnable(nullptr);
		EXPECT_TRUE(testIsTimerAlarmEnabled());
	}
}
