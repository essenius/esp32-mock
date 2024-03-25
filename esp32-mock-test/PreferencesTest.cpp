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
#include "../esp32-mock/Preferences.h"

namespace Esp32MockTest {
	TEST(PreferencesTest, InitTest) {
		Preferences preferences;
		EXPECT_TRUE(preferences.begin("test", false));
		preferences.clear();
		preferences.putString("dummy", "value");
		EXPECT_STREQ("value", preferences.getString("dummy").c_str());
		EXPECT_TRUE(preferences.isKey("dummy"));
		EXPECT_FALSE(preferences.isKey("bool"));

		EXPECT_FALSE(preferences.getBool("bool", false));
		preferences.putBool("bool", true);
		EXPECT_TRUE(preferences.getBool("bool", false));

		constexpr char Buffer[6]{0x31, 0x40, 0x25, 0x3a, 0x65, 0x7e};
		preferences.putBytes("bytes1", Buffer, 6);
		char buf[7];
		preferences.getBytes("bytes1", buf, sizeof buf);
		EXPECT_EQ(0, strncmp(buf, "1@%:e~", 6)) << "getBytes OK 2";
		preferences.getBytes("nonExisting", buf, sizeof buf);
		EXPECT_EQ(0, buf[0]);
		preferences.end();
		preferences.save();
		preferences.clear();
		EXPECT_FALSE(preferences.isKey("dummy"));
		preferences.load();
		EXPECT_FALSE(preferences.isKey("dummy"));
		preferences.begin("test", false);
		EXPECT_TRUE(preferences.isKey("dummy"));
		EXPECT_STREQ("value", preferences.getString("dummy").c_str());
		preferences.end();
	}

	TEST(PreferencesTest, NotStartedTest) {
		Preferences preferences;
		EXPECT_STREQ("", preferences.getString("dummy").c_str());

		preferences.putBytes("bytes", nullptr, 0);
		preferences.putString("string", nullptr);
		preferences.putUInt("uint", 0);
		EXPECT_FALSE(preferences.isKey("bytes"));
		EXPECT_FALSE(preferences.isKey("string"));
		EXPECT_FALSE(preferences.isKey("uint"));
	}

}
