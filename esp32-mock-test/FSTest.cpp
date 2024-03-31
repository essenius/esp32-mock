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
#include "../esp32-mock/FS.h"

namespace Esp32MockTest {
	TEST(FSTest, ReadTest) {
		constexpr auto FileName = "/ca.crt";
		File::deleteFiles();
		File::defineFile(FileName, "qwerty");

		auto file = SPIFFS.open(FileName, "r");
		EXPECT_FALSE(file);
		EXPECT_EQ(0, file.size());
		EXPECT_EQ(0, file.available());

		SPIFFS.begin();
		file = SPIFFS.open(FileName, "r");
		EXPECT_TRUE(file);

		EXPECT_EQ(6, file.size());
		EXPECT_EQ(6, file.available());
		auto content = file.readString();
		EXPECT_STREQ("qwerty", content.c_str());
		EXPECT_EQ(6, file.size());
		EXPECT_EQ(0, file.available());
		auto content2 = file.readString();
		EXPECT_STREQ("", content2.c_str());

		auto nonExistingFile = SPIFFS.open("/nonExisting.txt", "r");
		EXPECT_FALSE(nonExistingFile);
		EXPECT_EQ(0, nonExistingFile.size());
		EXPECT_EQ(0, nonExistingFile.available());
	}

	TEST(FSTest, WriteTest) {
		File::deleteFiles();
		SPIFFS.begin();
		constexpr auto FileName = "/data.crt";
        auto dataFile = SPIFFS.open(FileName, "w");
		EXPECT_TRUE(dataFile) << "write";
		EXPECT_EQ(0, dataFile.position()) << "write position";
		EXPECT_EQ(0, dataFile.size()) << "write size";
		EXPECT_EQ(0, dataFile.available()) << "write available";
		EXPECT_STREQ("", dataFile.readString().c_str()) << "write readString";
		dataFile.write("hello", 5);
		dataFile.close();
		dataFile = SPIFFS.open(FileName, "r");
		EXPECT_TRUE(dataFile) << "read";
		EXPECT_EQ(0, dataFile.position()) << "read position";
		EXPECT_EQ(5, dataFile.size()) << "read size";
		EXPECT_EQ(5, dataFile.available()) << "read available";
		EXPECT_STREQ("hello", dataFile.readString().c_str()) << "read readString";
		dataFile.write("not written", 11);
		EXPECT_EQ(5, dataFile.size()) << "read size";
		dataFile.close();
		dataFile = SPIFFS.open(FileName, "a+");
		EXPECT_TRUE(dataFile) << "append/update";
		EXPECT_EQ(5, dataFile.position()) << "append/update position";
		EXPECT_EQ(5, dataFile.size()) << "append/update size";
		EXPECT_EQ(0, dataFile.available()) << "append/update available";
		EXPECT_STREQ("", dataFile.readString().c_str()) << "append/update readString";
		dataFile.write(" there", 6);
		dataFile.close();
		dataFile = SPIFFS.open(FileName, "r+");
		EXPECT_TRUE(dataFile) << "read/update";
		EXPECT_EQ(0, dataFile.position()) << "read/update position";
		EXPECT_EQ(11, dataFile.size()) << "read/update size";
		EXPECT_EQ(11, dataFile.available()) << "read/update available";
		EXPECT_STREQ("hello there", dataFile.readString().c_str()) << "read/update readString";
		dataFile.seek(0, SeekSet);
		dataFile.write("new", 3);
		dataFile.close();

	    dataFile = SPIFFS.open(FileName, "r");
		EXPECT_TRUE(dataFile) << "read 2";
		EXPECT_EQ(0, dataFile.position()) << "read 2 position";
		EXPECT_EQ(11, dataFile.size()) << "read 2 size";
		EXPECT_EQ(11, dataFile.available()) << "read 2 available";
		EXPECT_STREQ("newlo there", dataFile.readString().c_str()) << "read 2 readString";
		dataFile.close();

		dataFile = SPIFFS.open(FileName, "w+");
		EXPECT_TRUE(dataFile) << "write/update";
		EXPECT_EQ(0, dataFile.position()) << "write/update position";
		EXPECT_EQ(0, dataFile.size()) << "write/update size";
		EXPECT_EQ(0, dataFile.available()) << "write/update available";
		EXPECT_STREQ("", dataFile.readString().c_str()) << "write/update readString";
		dataFile.close();


	}

	TEST(FSTest, DirTest) {
		File::deleteFiles();
		File::defineFile("/1/test", "qwerty");
		File::defineFile("/1/test2", "abc");
		File::defineFile("/2/test", "q");
		SPIFFS.begin();
		auto dir = SPIFFS.openDir("/");
		EXPECT_TRUE(dir.next());
		EXPECT_STREQ("/1/test", dir.fileName().c_str());
		EXPECT_EQ(6, dir.fileSize());
		EXPECT_TRUE(dir.next());
		EXPECT_STREQ("/1/test2", dir.fileName().c_str());
		EXPECT_EQ(3, dir.fileSize());
		EXPECT_TRUE(dir.next());
		EXPECT_STREQ("/2/test", dir.fileName().c_str());
		EXPECT_EQ(1, dir.fileSize());
		EXPECT_FALSE(dir.next());

		dir = SPIFFS.openDir("/2/");
		EXPECT_TRUE(dir.next());
		EXPECT_STREQ("/2/test", dir.fileName().c_str());
		EXPECT_EQ(1, dir.fileSize());
		EXPECT_FALSE(dir.next());
	}

	TEST(FSTest, SeekTest) {
		constexpr auto FileName = "/ca.crt";
		SPIFFS.begin();
		File::deleteFiles();
		File::defineFile(FileName, "qwerty");
		auto file = SPIFFS.open(FileName, "a+");
		EXPECT_TRUE(file) << "File open";
		EXPECT_EQ(6, file.position()) << "open";

		file.seek(-2, SeekCur);
		EXPECT_EQ(4, file.position()) << "SeekCur-2";
		EXPECT_STREQ("ty", file.readString().c_str()) << "SeekCur-2";
		file.seek(0, SeekSet);
		EXPECT_EQ(0, file.position());
		file.seek(-3, SeekEnd);
		EXPECT_EQ(3, file.position()) << "SeekEnd-3";
		EXPECT_STREQ("rty", file.readString().c_str()) << "SeekEnd-3";
		file.seek(0, static_cast<SeekMode>(3));
		EXPECT_EQ(6, file.position()) << "Invalid seek doesn't change position";


	}

	TEST(FSTest, WrongModeTest) {
		constexpr auto FileName = "/ca.crt";
		SPIFFS.begin();
        const auto file = SPIFFS.open(FileName, "q");
		EXPECT_FALSE(file);
	}

}