// Create a set of test cases for thje ESP class

#include <gtest/gtest.h>
#include "../esp32-mock/ESP.h"

namespace Esp32MockTest {
	TEST(ESPTest, InitTest) {
		EXPECT_DOUBLE_EQ(3.1415926535897932384626433832795, PI);

		ESP.restart();
		EXPECT_EQ(32000L, ESP.getFreeHeap());
		EXPECT_EQ(29000L, ESP.getFreeHeap());
		EXPECT_EQ(26000L, ESP.getFreeHeap());
		EXPECT_EQ(23000L, ESP.getFreeHeap());
		EXPECT_EQ(20000L, ESP.getFreeHeap());
		EXPECT_EQ(17000L, ESP.getFreeHeap());
		EXPECT_EQ(14000L, ESP.getFreeHeap());
		EXPECT_EQ(14000L, ESP.getFreeHeap());
		EXPECT_EQ(11000L, ESP.getFreeHeap());
		EXPECT_EQ(32000L, ESP.getFreeHeap());
	}

	TEST(ESPTest, DigitalReadWriteTest) {
		EXPECT_EQ(LOW, digitalRead(0));
		digitalWrite(0, HIGH);
		EXPECT_EQ(HIGH, digitalRead(0));
		EXPECT_EQ(INPUT, getPinMode(0));
		pinMode(0, OUTPUT);
		EXPECT_EQ(OUTPUT, getPinMode(0));
	}

	TEST(ESPTest, realtimeOfftimeTest) {
		setRealTime(false);
		configTime(0, 0, nullptr, nullptr);
		EXPECT_EQ(50, micros());
		EXPECT_EQ(100, micros());
		delayMicroseconds(1000);
		EXPECT_EQ(1150, micros());
		EXPECT_EQ(1, millis());
	}

	TEST(ESPTest, realtimeOntimeTest) {
		setRealTime(true);
		EXPECT_EQ(0, millis());
		delay(5);
		auto currentTime = micros();
		EXPECT_TRUE(currentTime >= 5000L && currentTime < 5200);
		disableDelay(true);
		delay(5);
		currentTime = micros();
		EXPECT_TRUE(currentTime >= 5000L && currentTime < 5200);
		disableDelay(false);
		delay(5);
		currentTime = micros();
		EXPECT_TRUE(currentTime >= 10000L && currentTime < 10200);
		currentTime	= micros();
		delayMicroseconds(100);
		EXPECT_TRUE(micros() >= currentTime + 100);
		shiftMicros(200);
		currentTime = micros();
		EXPECT_TRUE(currentTime >= 200 && currentTime < 300);
		setRealTime(false);
	}

	TEST(ESPTest, SerialTest) {
		Serial.begin(115200);
		EXPECT_STREQ("", Serial.getOutput());
		Serial.print("Hello");
		EXPECT_STREQ("Hello", Serial.getOutput());
		Serial.println(" There");
		EXPECT_STREQ("Hello There\n", Serial.getOutput());
		EXPECT_EQ(0, Serial.available());
		EXPECT_EQ(0, Serial.read());
		Serial.setInput("Hello");
		EXPECT_EQ('H', Serial.read());
		EXPECT_EQ('e', Serial.read());
		EXPECT_EQ(3, Serial.available());
	}

	TEST(ESPTest, LogLevelTest) {
		EXPECT_STREQ("E", toString(LogLevel::Error));
		EXPECT_STREQ("W", toString(LogLevel::Warning));
		EXPECT_STREQ("I", toString(LogLevel::Info));
		EXPECT_STREQ("D", toString(LogLevel::Debug));
		EXPECT_STREQ("V", toString(LogLevel::Verbose));
	}

	TEST(ESPTest, TimerTest) {
		const auto result  = timerBegin(1, 2, true);
		EXPECT_EQ(1, result->num);
		EXPECT_EQ(2, result->group);
		timerEnd(result);
		EXPECT_EQ(255, result->num);
		EXPECT_EQ(255, result->group);
	}
}