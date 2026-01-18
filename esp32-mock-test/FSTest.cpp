// Copyright 2024-2026 Rik Essenius
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

namespace esp32_mock_test {
	class FSTest : public testing::Test {
	public:
		void SetUp() override {
			File::testDeleteFiles();
		}

		static void expectNextChar(File& file, const char expected, const int remaining) {
			const auto ch = file.read();
			EXPECT_EQ(ch, expected) << "Next character for " << remaining;
			EXPECT_EQ(file.available(), remaining) << "Available after reading character " << remaining;
		}

		static void expectNextFile(Dir& dir, const char* expectedFileName, const size_t expectedSize) {
			EXPECT_TRUE(dir.next());
			EXPECT_STREQ(dir.fileName().c_str(), expectedFileName);
			EXPECT_EQ(dir.fileSize(), expectedSize);
		}

		static void expectFileMetadata(const File& file, const bool isOpen, const size_t expectedSize, const size_t expectedAvailable,
		                               const char* label) {
			if (isOpen) {
				EXPECT_TRUE(file) << "File should be valid for " << label;
			}
			else {
				EXPECT_FALSE(file) << "File should be invalid for " << label;
			}
			EXPECT_EQ(file.size(), expectedSize) << "File size for " << label;
			EXPECT_EQ(file.available(), expectedAvailable) << "File available for " << label;
		}

		static constexpr auto kFileName = "/ca.crt";
	};

	TEST_F(FSTest, OpenTest) {
		File::testDefineFile(kFileName, "qwerty");

		auto file = SPIFFS.open(kFileName, "r");
		expectFileMetadata(file, false, 0, 0, "before begin");

		SPIFFS.begin();
		file = SPIFFS.open(kFileName, "r");
		expectFileMetadata(file, true, 6, 6, "after begin");
		file.close();

		auto writeFile = SPIFFS.open(kFileName, "w");
		expectFileMetadata(writeFile, true, 0, 0, "write mode");
		EXPECT_EQ(writeFile.position(), 0) << "write position 0";
		writeFile.close();

		const auto nonExistingFile = SPIFFS.open("/nonExisting.txt", "r");
		expectFileMetadata(nonExistingFile, false, 0, 0, "non-existing file");
	}

	TEST_F(FSTest, ReadStringTest) {
		File::testDefineFile(kFileName, "qwerty");
		SPIFFS.begin();
		auto file = SPIFFS.open(kFileName, "r");
		const auto content = file.readString();
		EXPECT_STREQ(content.c_str(), "qwerty");
		expectFileMetadata(file, true, 6, 0, "after readString");
		const auto content2 = file.readString();
		EXPECT_STREQ(content2.c_str(), "");
		file.close();
	}

	TEST_F(FSTest, ReadCharTest) {
		File::testDefineFile(kFileName, "abc");
		auto file = SPIFFS.open(kFileName, "r");
		SPIFFS.begin();
		file = SPIFFS.open(kFileName, "r");
		expectNextChar(file, 'a', 2);
		expectNextChar(file, 'b', 1);
		expectNextChar(file, 'c', 0);
		EXPECT_EQ(file.read(), -1) << "EOF";
		EXPECT_EQ(file.available(), 0);
		file.close();
	}

	TEST_F(FSTest, ReadTest) {
		File::testDefineFile(kFileName, "abc");
		auto file = SPIFFS.open(kFileName, "r");
		SPIFFS.begin();
		file = SPIFFS.open(kFileName, "r");
		uint8_t buffer[10] = {};
		size_t bytesRead = file.read(buffer, 2);
		EXPECT_EQ(bytesRead, 2) << "bytes read 2";
		EXPECT_EQ(buffer[0], 'a') << "first char";
		EXPECT_EQ(buffer[1], 'b') << "second char";

		bytesRead = file.read(buffer, 2);
		EXPECT_EQ(bytesRead, 1) << "bytes read 1";
		EXPECT_EQ(buffer[0], 'c') << "third char";

		EXPECT_EQ(file.read(), -1) << "EOF";
		EXPECT_EQ(file.available(), 0);
		file.close();
	}

	TEST_F(FSTest, WriteTest) {
		SPIFFS.begin();
		auto dataFile = SPIFFS.open(kFileName, "w");

		EXPECT_STREQ(dataFile.readString().c_str(), "") << "write readString empty";
		dataFile.write("hello", 5);
		dataFile.write('!');
		EXPECT_EQ(dataFile.read(), -1) << "cannot read char from a write-only file";
		uint8_t buffer[2];
		EXPECT_EQ(dataFile.read(buffer, 2), 0) << "cannot read uint8_t buffer from a write-only file";

		dataFile.close();
		dataFile = SPIFFS.open(kFileName, "r");
		EXPECT_TRUE(dataFile) << "open for read";
		expectFileMetadata(dataFile, true, 6, 6, "read");
		EXPECT_EQ(dataFile.position(), 0) << "read position";
		EXPECT_STREQ(dataFile.readString().c_str(), "hello!") << "read readString";
		EXPECT_EQ(dataFile.write("not written", 11), 0) << "write in read mode";
		EXPECT_EQ(dataFile.size(), 6) << "read size";
		dataFile.close();

		dataFile = SPIFFS.open(kFileName, "a+");
		expectFileMetadata(dataFile, true, 6, 0, "append/update");
		EXPECT_EQ(dataFile.position(), 6) << "append/update position";
		EXPECT_STREQ(dataFile.readString().c_str(), "") << "append/update readString";
		constexpr uint8_t moreData[] = " there";
		EXPECT_EQ(dataFile.write(moreData, 6), 6) << "append/update write with uint8_t*";
		dataFile.close();

		dataFile = SPIFFS.open(kFileName, "r+");
		expectFileMetadata(dataFile, true, 12, 12, "read/update");
		EXPECT_EQ(dataFile.position(), 0) << "read/update position";
		EXPECT_STREQ(dataFile.readString().c_str(), "hello! there") << "read/update readString";
		dataFile.seek(0, SeekSet);
		dataFile.write("new", 3);
		dataFile.close();

		dataFile = SPIFFS.open(kFileName, "r");
		expectFileMetadata(dataFile, true, 12, 12, "read 2");
		EXPECT_EQ(dataFile.position(), 0) << "read 2 position";
		EXPECT_STREQ(R"(newlo! there)", dataFile.readString().c_str()) << "read 2 readString";
		dataFile.close();

		dataFile = SPIFFS.open(kFileName, "w+");
		expectFileMetadata(dataFile, true, 0, 0, "write/update");
		EXPECT_EQ(dataFile.position(), 0) << "write/update position";
		EXPECT_STREQ(dataFile.readString().c_str(), "") << "write/update readString";
		dataFile.close();
	}

	TEST_F(FSTest, DirTest) {
		File::testDefineFile("/1/test", "qwerty");
		File::testDefineFile("/1/test2", "abc");
		File::testDefineFile("/2/test", "q");
		SPIFFS.begin();
		auto dir = SPIFFS.openDir("/");
		expectNextFile(dir, "/1/test", 6);
		expectNextFile(dir, "/1/test2", 3);
		expectNextFile(dir, "/2/test", 1);
		EXPECT_FALSE(dir.next());

		dir = SPIFFS.openDir("/2/");
		expectNextFile(dir, "/2/test", 1);
		EXPECT_FALSE(dir.next());
	}

	TEST_F(FSTest, SeekTest) {
		constexpr auto fileName = "/ca.crt";
		SPIFFS.begin();
		File::testDeleteFiles();
		File::testDefineFile(fileName, "qwerty");
		auto file = SPIFFS.open(fileName, "a+");
		EXPECT_TRUE(file) << "File open";
		EXPECT_EQ(file.position(), 6) << "open";

		file.seek(-2, SeekCur);
		EXPECT_EQ(file.position(), 4) << "SeekCur-2";
		EXPECT_STREQ(file.readString().c_str(), "ty") << "SeekCur-2";
		file.seek(0, SeekSet);
		EXPECT_EQ(file.position(), 0);
		file.seek(-3, SeekEnd);
		EXPECT_EQ(file.position(), 3) << "SeekEnd-3";
		EXPECT_STREQ(file.readString().c_str(), "rty") << "SeekEnd-3";
		file.seek(0, static_cast<SeekMode>(4));
		EXPECT_EQ(file.position(), 6) << "Invalid seek doesn't change position";
		file.seek(20, SeekEnd);
		EXPECT_EQ(file.position(), 6) << "Seek past EOF returns last position";
	}

	TEST_F(FSTest, WrongModeTest) {
		constexpr auto FileName = "/ca.crt";
		SPIFFS.begin();
		const auto file = SPIFFS.open(FileName, "q");
		EXPECT_FALSE(file);
	}
}
