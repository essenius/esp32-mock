// Copyright 2024-2025 Rik Essenius
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
#include "../esp32-mock/EEPROM.h"

namespace Esp32MockTest {
	TEST(EEPROMTest, ReadWriteTest) {
		EEPROM.reset(); // forget the previous content - not part of the standard interface
		EEPROM.begin(512);
		EXPECT_EQ(0, EEPROM.read(100)) << "Initial value is 0";
		EEPROM.write(100, 'a');
		EXPECT_EQ('a', EEPROM.read(100)) << "Value after write";
		EEPROM.end();
		EEPROM.begin(512);
		EXPECT_EQ('a', EEPROM.read(100)) << "Value after reopen retained";
		EEPROM.end();
	}

		TEST(EEPROMTest, GetPutTest) {
		EEPROM.reset(); 
		EEPROM.begin(512);
		uint16_t value = 65535;
		EEPROM.get(100, value);
		EXPECT_EQ(0, value) << "Initial value is 0";
		value = 12345;
		EEPROM.put(100, value);
		EXPECT_TRUE(EEPROM.isDirty()) << "isDirty true after put";
		value = 0;
		EEPROM.get(100, value);
		EXPECT_EQ(12345, value)  << "get returns the put value before commit";
		EEPROM.commit();
		EXPECT_FALSE(EEPROM.isDirty()) << "isDirty false after commit";
		value = 0;
		EEPROM.get(100, value);
		EXPECT_EQ(12345, value) << "Value after commit is the same";
		EEPROM.end();
		EEPROM.begin(512);
		value = 0;
		EEPROM.get(100, value);
		EXPECT_EQ(12345, value) << "Value after reopen retained";

		EEPROM.put(100, 32767);
		EXPECT_TRUE(EEPROM.isDirty()) << "isDirty true after put before implicit commit";		
		EEPROM.end();	
		EXPECT_FALSE(EEPROM.isDirty()) << "isDirty false after implicit commit";
		
		EEPROM.get(100, value);
		EXPECT_EQ(32767, value) << "Past put value after reopen saved by implicit commit";}
}