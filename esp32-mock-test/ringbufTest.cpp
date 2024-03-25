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
#include "../esp32-mock/freertos/ringbuf.h"

namespace Esp32MockTest {
	TEST(RingBufTest, InitTest) {
		uxRingbufReset();
		EXPECT_EQ(pdFALSE, xRingbufferReceiveSplit(nullptr, nullptr, nullptr, nullptr, nullptr, 0));
		EXPECT_EQ(0, xRingbufferGetCurFreeSize(nullptr));
		EXPECT_EQ(pdFALSE, xRingbufferSend(nullptr, nullptr, 0, 0));

		const auto handle = xRingbufferCreate(100, RINGBUF_TYPE_ALLOWSPLIT);
		EXPECT_EQ(12800, xRingbufferGetCurFreeSize(handle));

		char* item1 = nullptr;
		char* item2 = nullptr;
		size_t item1Size;
		size_t item2Size;
		constexpr char SinglePayload[] = R"(ABCDEFGHIJKLMNOPQRSTUVWXYZ)";
		constexpr char DoublePayload[] = R"(ABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890abcdefghijklmnopqrstuvwxyz!@#$%^&*())";
		EXPECT_EQ(pdFALSE, xRingbufferReceiveSplit(handle, reinterpret_cast<void**>(&item1), reinterpret_cast<void**>(&item2), &item1Size, &item2Size, 0));
		EXPECT_EQ(pdFALSE, xRingbufferSend(handle, DoublePayload, 132, 0));
		EXPECT_EQ(pdTRUE, xRingbufferSend(handle, SinglePayload , sizeof SinglePayload, 0));
		EXPECT_EQ(12672, xRingbufferGetCurFreeSize(handle));
		EXPECT_EQ(pdTRUE, xRingbufferSend(handle, DoublePayload, sizeof DoublePayload, 0));
		EXPECT_EQ(12544, xRingbufferGetCurFreeSize(handle));
		EXPECT_EQ(pdTRUE, xRingbufferReceiveSplit(handle, reinterpret_cast<void**>(&item1), reinterpret_cast<void**>(&item2), &item1Size, &item2Size, 0));
		EXPECT_EQ(27, item1Size);
		EXPECT_EQ(0, item2Size);
		EXPECT_EQ(0, strncmp(SinglePayload, item1, item1Size));
		EXPECT_EQ(12544, xRingbufferGetCurFreeSize(handle));

		EXPECT_EQ(pdTRUE, xRingbufferReceiveSplit(handle, reinterpret_cast<void**>(&item1), reinterpret_cast<void**>(&item2), &item1Size, &item2Size, 0));
		EXPECT_EQ(64, item1Size);
		EXPECT_EQ(10, item2Size);
		EXPECT_EQ(0, strncmp(DoublePayload, item1, item1Size));
		EXPECT_EQ(0, strncmp(DoublePayload + item1Size, item2, item2Size));
		EXPECT_EQ(12544, xRingbufferGetCurFreeSize(handle));
	}

	TEST(RingBufTest, BufferFullTest) {
		const auto handle = xRingbufferCreate(100, RINGBUF_TYPE_ALLOWSPLIT);
		setRingBufferBufferFull(handle, true);
		EXPECT_EQ(7, xRingbufferGetCurFreeSize(handle));
	}

	TEST(RingBufTest, NoMoreEntriesTest) {
		const auto handle = xRingbufferCreate(100, RINGBUF_TYPE_ALLOWSPLIT);
		setRingBufferNoMoreEntries(handle);
		EXPECT_EQ(pdFALSE, xRingbufferSend(handle, nullptr, 0,0));
	}

	TEST(RingBufTest, BufferOverrunTest) {
		uxRingbufReset();
		for (int i=0; i<10;i++) {
			EXPECT_NE(nullptr, xRingbufferCreate(1, RINGBUF_TYPE_ALLOWSPLIT)) << "attempt " << i;
		}
		EXPECT_EQ(nullptr, xRingbufferCreate(1, RINGBUF_TYPE_ALLOWSPLIT));
	}
}
