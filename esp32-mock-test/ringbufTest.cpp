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
#include "../esp32-mock/freertos/ringbuf.h"

namespace esp32_mock_test {
	TEST(RingBufTest, InitTest) {
		test_uxRingbufReset();
		EXPECT_EQ(xRingbufferReceiveSplit(nullptr, nullptr, nullptr, nullptr, nullptr, 0), pdFALSE);
		EXPECT_EQ(xRingbufferGetCurFreeSize(nullptr), 0);
		EXPECT_EQ(xRingbufferSend(nullptr, nullptr, 0, 0), pdFALSE);

		const auto handle = xRingbufferCreate(100, RINGBUF_TYPE_ALLOWSPLIT);
		EXPECT_EQ(xRingbufferGetCurFreeSize(handle), 12800);

		char* item1 = nullptr;
		char* item2 = nullptr;
		size_t item1Size;
		size_t item2Size;
		constexpr char singlePayload[] = R"(ABCDEFGHIJKLMNOPQRSTUVWXYZ)";
		constexpr char doublePayload[] = R"(ABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890abcdefghijklmnopqrstuvwxyz!@#$%^&*())";
		EXPECT_EQ(xRingbufferReceiveSplit(handle, reinterpret_cast<void**>(&item1), reinterpret_cast<void**>(&item2), &item1Size, &item2Size, 0), pdFALSE);
		EXPECT_EQ(xRingbufferSend(handle, doublePayload, 132, 0), pdFALSE);
		EXPECT_EQ(xRingbufferSend(handle, singlePayload , sizeof singlePayload, 0), pdTRUE);
		EXPECT_EQ(xRingbufferGetCurFreeSize(handle), 12672);
		EXPECT_EQ(xRingbufferSend(handle, doublePayload, sizeof doublePayload, 0), pdTRUE);
		EXPECT_EQ(xRingbufferGetCurFreeSize(handle), 12544);
		EXPECT_EQ(xRingbufferReceiveSplit(handle, reinterpret_cast<void**>(&item1), reinterpret_cast<void**>(&item2), &item1Size, &item2Size, 0), pdTRUE);
		EXPECT_EQ(item1Size, 27);
		EXPECT_EQ(item2Size, 0);
		EXPECT_EQ(strncmp(singlePayload, item1, item1Size), 0);
		EXPECT_EQ(xRingbufferGetCurFreeSize(handle), 12544);

		EXPECT_EQ(xRingbufferReceiveSplit(handle, reinterpret_cast<void**>(&item1), reinterpret_cast<void**>(&item2), &item1Size, &item2Size, 0), pdTRUE);
		EXPECT_EQ(item1Size, 64);
		EXPECT_EQ(item2Size, 10);
		EXPECT_EQ(strncmp(doublePayload, item1, item1Size), 0);
		EXPECT_EQ(strncmp(doublePayload + item1Size, item2, item2Size), 0);
		EXPECT_EQ(xRingbufferGetCurFreeSize(handle), 12544);
	}

	TEST(RingBufTest, BufferFullTest) {
		const auto handle = xRingbufferCreate(100, RINGBUF_TYPE_ALLOWSPLIT);
		test_set_ring_buffer_buffer_full(handle, true);
		EXPECT_EQ(xRingbufferGetCurFreeSize(handle), 7);
	}

	TEST(RingBufTest, NoMoreEntriesTest) {
		const auto handle = xRingbufferCreate(100, RINGBUF_TYPE_ALLOWSPLIT);
		test_set_ring_buffer_no_more_entries(handle);
		EXPECT_EQ(xRingbufferSend(handle, nullptr, 0,0), pdFALSE);
	}

	TEST(RingBufTest, BufferOverrunTest) {
		test_uxRingbufReset();
		for (int i=0; i<10;i++) {
			EXPECT_NE(nullptr, xRingbufferCreate(1, RINGBUF_TYPE_ALLOWSPLIT)) << "attempt " << i;
		}
		EXPECT_EQ(xRingbufferCreate(1, RINGBUF_TYPE_ALLOWSPLIT), nullptr);
	}
}
