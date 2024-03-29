// Copyright 2024 Rik Essenius
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
	TEST(EEPROMTest, BasicTest) {
		EEPROM.reset(); // forget the previous content - not part of the standard interface
		EEPROM.begin(512);
		EXPECT_EQ(0, EEPROM.read(100));
		EEPROM.write(100, 'a');
		EXPECT_EQ('a', EEPROM.read(100));
		EEPROM.end();
		EEPROM.begin(512);
		EXPECT_EQ('a', EEPROM.read(100));
		EEPROM.end();
	}
}