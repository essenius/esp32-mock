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
		class FSTest : public testing::Test {
	public:

		void SetUp() override {
			File::deleteFiles();
		}

		void expectNextChar(File& file, const char expected, const int remaining) const {
			const auto ch = file.read();
			EXPECT_EQ(expected, ch) << "Next character for " << remaining;
			EXPECT_EQ(remaining, file.available()) << "Available after reading character " << remaining;
		}

		void expectFileMetadata(const File& file, bool isOpen, const size_t expectedSize, const size_t expectedAvailable, const char* label) {
			if (isOpen) {
				EXPECT_TRUE(file) << "File should be valid for " << label;
			} else {
				EXPECT_FALSE(file) << "File should be invalid for " << label;
			}
			EXPECT_EQ(expectedSize, file.size()) << "File size for " << label;
			EXPECT_EQ(expectedAvailable, file.available()) << "File available for " << label;
		}

		static constexpr const char* FileName = "/ca.crt";
	};
	
	TEST_F(FSTest, OpenTest) {
		File::defineFile(FileName, "qwerty");

		auto file = SPIFFS.open(FileName, "r");
		expectFileMetadata(file, false, 0, 0, "before begin");

		SPIFFS.begin();
		file = SPIFFS.open(FileName, "r");
		expectFileMetadata(file, true, 6, 6, "after begin");
		file.close();

		auto writeFile = SPIFFS.open(FileName, "w");
		expectFileMetadata(writeFile, true, 0, 0, "write mode");
		EXPECT_EQ(0, writeFile.position()) << "write position 0";
		writeFile.close();

		auto nonExistingFile = SPIFFS.open("/nonExisting.txt", "r");
		expectFileMetadata(nonExistingFile, false, 0, 0, "non-existing file");
	};

	TEST_F(FSTest, ReadStringTest) {
		File::defineFile(FileName, "qwerty");
		auto file = SPIFFS.open(FileName, "r");
		SPIFFS.begin();
		file = SPIFFS.open(FileName, "r");
		auto content = file.readString();
		EXPECT_STREQ("qwerty", content.c_str());
		expectFileMetadata(file, true, 6, 0, "after readString");
		auto content2 = file.readString();
		EXPECT_STREQ("", content2.c_str());
		file.close();
	}

	TEST_F(FSTest, ReadTest) {
		File::defineFile(FileName, "abc");
		auto file = SPIFFS.open(FileName, "r");
		SPIFFS.begin();
		file = SPIFFS.open(FileName, "r");
		expectNextChar(file, 'a', 2);
		expectNextChar(file, 'b', 1);
		expectNextChar(file, 'c', 0);
		EXPECT_EQ(-1, file.read()) << "EOF";
		EXPECT_EQ(0, file.available());
		file.close();
	}

	TEST_F(FSTest, WriteTest) {
		SPIFFS.begin();
        auto dataFile = SPIFFS.open(FileName, "w");

		EXPECT_STREQ("", dataFile.readString().c_str()) << "write readString empty";
		dataFile.write("hello", 5);
		dataFile.close();
		dataFile = SPIFFS.open(FileName, "r");
		EXPECT_TRUE(dataFile) << "open for read";
		expectFileMetadata(dataFile, true, 5, 5, "read");
		EXPECT_EQ(0, dataFile.position()) << "read position";
		EXPECT_STREQ("hello", dataFile.readString().c_str()) << "read readString";
		EXPECT_EQ(0, dataFile.write("not written", 11)) << "write in read mode";
		EXPECT_EQ(5, dataFile.size()) << "read size";
		dataFile.close();

		dataFile = SPIFFS.open(FileName, "a+");
		expectFileMetadata(dataFile, true, 5, 0, "append/update");
		EXPECT_EQ(5, dataFile.position()) << "append/update position";
		EXPECT_STREQ("", dataFile.readString().c_str()) << "append/update readString";
		const uint8_t moreData[] = " there";
		EXPECT_EQ(6, dataFile.write(moreData, 6)) << "append/update write with uint8_t*";
		dataFile.close();

		dataFile = SPIFFS.open(FileName, "r+");
		expectFileMetadata(dataFile, true, 11, 11, "read/update");
		EXPECT_EQ(0, dataFile.position()) << "read/update position";
		EXPECT_STREQ("hello there", dataFile.readString().c_str()) << "read/update readString";
		dataFile.seek(0, SeekSet);
		dataFile.write("new", 3);
		dataFile.close();

	    dataFile = SPIFFS.open(FileName, "r");
		expectFileMetadata(dataFile, true, 11, 11, "read 2");
		EXPECT_EQ(0, dataFile.position()) << "read 2 position";
		EXPECT_STREQ("newlo there", dataFile.readString().c_str()) << "read 2 readString";
		dataFile.close();

		dataFile = SPIFFS.open(FileName, "w+");
		expectFileMetadata(dataFile, true, 0, 0, "write/update");
		EXPECT_EQ(0, dataFile.position()) << "write/update position";
		EXPECT_STREQ("", dataFile.readString().c_str()) << "write/update readString";
		dataFile.close();
	}

	TEST_F(FSTest, DirTest) {
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

	TEST_F(FSTest, SeekTest) {
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

	TEST_F(FSTest, WrongModeTest) {
		constexpr auto FileName = "/ca.crt";
		SPIFFS.begin();
        const auto file = SPIFFS.open(FileName, "q");
		EXPECT_FALSE(file);
	}
}