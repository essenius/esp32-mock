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

#include <fstream>
#include <gtest/gtest.h>
#include "../esp32-mock/Preferences.h"

namespace esp32_mock_test {
	TEST(PreferencesTest, InitTest) {
		Preferences preferences;
		preferences.testReset();
		EXPECT_STREQ(preferences.getString("dummy").c_str(), "") << "getString before begin returns empty string";		
		EXPECT_TRUE(preferences.begin("test", false)) << "begin succeeds";
		preferences.clear();
		preferences.putString("dummy", "value");
		EXPECT_STREQ(preferences.getString("dummy").c_str(), "value") << "getString retrieves value set by putString";
		EXPECT_TRUE(preferences.isKey("dummy")) << "dummy is a key";
		EXPECT_FALSE(preferences.isKey("bool")) << "bool is not a key";

		EXPECT_FALSE(preferences.getBool("bool", false)) << "getBool with default false on non-existing key returns false";
		preferences.putBool("bool", true);
		EXPECT_TRUE(preferences.getBool("bool", false)) << "getBool with default false on existing key returns the existing value";

		constexpr char buffer[6]{0x31, 0x40, 0x25, 0x3a, 0x65, 0x7e};
		preferences.putBytes("bytes1", buffer, 6);
		char buf[7];
		preferences.getBytes("bytes1", buf, sizeof buf);
		EXPECT_EQ(strncmp(buf, "1@%:e~", 6), 0) << "getBytes returns values set by putBytes";
		preferences.getBytes("nonExisting", buf, sizeof buf);
		preferences.putString("multiline", "multi\nline\nvalue\n");
		preferences.putBool("zbool2", false);
		// the value is the special value denoting a multiline. It should just give back this value
		preferences.putString("PseudoMultiline", "#10");
		preferences.end();
		preferences.clear();
		EXPECT_FALSE(preferences.isKey("dummy")) << "dummy does not exist after clearing";
		preferences.begin("test", false);
		EXPECT_TRUE(preferences.isKey("dummy")) << "dummy exists after begin with test";
		EXPECT_STREQ(preferences.getString("dummy").c_str(), "value") << "getString returns the right value";
		EXPECT_STREQ(preferences.getString("multiline").c_str(), "multi\nline\nvalue\n") << "Multiline getString returns the right value";
		EXPECT_FALSE(preferences.getBool("zbool2", true)) << "getBool on zbool2 returns its value, not the default";
		EXPECT_STREQ(preferences.getString("PseudoMultiline").c_str(), "#10") << "Pseudo-Multiline OK";
		preferences.end();
		preferences.testReset();
	}

	TEST(PreferencesTest, NotStartedTest) {
		Preferences preferences;
		EXPECT_STREQ(preferences.getString("dummy").c_str(), "") << "getString before begin returns empty string";

		preferences.putBytes("bytes", nullptr, 0);
		preferences.putString("string", nullptr);
		preferences.putUInt("uint", 0);
		EXPECT_FALSE(preferences.isKey("bytes")) << "putBytes before begin does not store value";
		EXPECT_FALSE(preferences.isKey("string")) << "putString before begin does not store value";
		EXPECT_FALSE(preferences.isKey("uint")) << "putUInt before begin does not store value";
	}

	TEST(PreferencesTest, WrongPreferencesTest) {
		std::ofstream stream;
		stream.open("preferences.txt");
		stream << "test\nmultiline=#3\noneValue\n";
		stream.close();
		Preferences preferences;
		preferences.begin("test");
		EXPECT_STREQ(preferences.getString("multiline").c_str(), "oneValue\n") << "Multiline with insufficient lines at the end recovers";
		preferences.end();
		preferences.testReset();
	}

}
