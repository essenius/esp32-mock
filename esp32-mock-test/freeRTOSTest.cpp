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
#include "../esp32-mock/freertos/freeRTOS.h"

namespace esp32_mock_test {
	class FreeRtosTest : public testing::Test {
	public:
		QueueHandle_t queue[kMaxQueues] = {nullptr};
		char buffer[10] = "x";
	};

	TEST_F(FreeRtosTest, QueueTest) {
		testUxQueueReset();
		for (auto& i : queue) {
			i = xQueueCreate(kMaxQueues, 2);
			EXPECT_NE(nullptr, i) << "Not null";
		}
		EXPECT_EQ(xQueueCreate(kMaxQueues, 2), nullptr) << "Next returns nullptr";
		EXPECT_EQ(uxQueueSpacesAvailable(queue[0]), 20ul) << "queue 0 has 20 spaces left at start";
		EXPECT_EQ(uxQueueMessagesWaiting(queue[0]), 0ul) << "queue 0 has 0 waiting message at start";
		EXPECT_EQ(xQueueReceive(queue[0], buffer, 0), pdFALSE) << "Nothing in the queue";
		EXPECT_EQ(xQueueSendToFront(queue[0], buffer, 0), pdFALSE) << "Not implemented";
		EXPECT_EQ(xQueueSendToBack(queue[0], buffer, 0), pdTRUE) << "Item 1 sent";
		EXPECT_EQ(uxQueueSpacesAvailable(queue[0]), 19ul) << "queue 0 has 19 spaces left";
		EXPECT_EQ(uxQueueMessagesWaiting(queue[0]), 1ul) << "queue 0 has 1 waiting message";
		buffer[0] = 'y';
		EXPECT_EQ(xQueueSendToBack(queue[0], buffer, 0), pdTRUE) << "Item 2 sent";
		EXPECT_EQ(uxQueueMessagesWaiting(queue[0]), 2ul) << "queue 0 has 2 waiting message";

		buffer[0] = '\0';
		EXPECT_EQ(xQueueReceive(queue[0], buffer, 0), pdTRUE) << "Item read";
		EXPECT_STREQ(buffer, "x") << "read item has right value";
		EXPECT_EQ(uxQueueMessagesWaiting(queue[0]), 1ul) << "queue 0 has 1 waiting message after read";
		buffer[0] = '\0';
		EXPECT_EQ(xQueueReceive(queue[0], buffer, 0), pdTRUE) << "Item read";
		EXPECT_STREQ(buffer, "y") << "read item has right value";
		EXPECT_EQ(uxQueueSpacesAvailable(queue[0]), 20ul) << "queue 0 has 20 spaces left after reads";
		EXPECT_EQ(uxQueueMessagesWaiting(queue[0]), 0ul) << "queue 0 has 0 waiting message after read";

		EXPECT_EQ(xQueueSendToBack(queue[1], buffer, 0), pdTRUE) << "Item sent to queue 1";
		EXPECT_EQ(xQueueSendToBack(queue[3], buffer, 0), pdTRUE) << "Item sent to queue 3";
		testUxQueueReset();
		EXPECT_NE(nullptr, xQueueCreate(5, 2)) << "Can create new queue after reset";
		testUxQueueReset();
	}

	TEST_F(FreeRtosTest, WatermarkTest) {
		uxTaskGetStackHighWaterMarkReset();
		const auto handle = xTaskGetCurrentTaskHandle();
		EXPECT_EQ(uxTaskGetStackHighWaterMark(handle), 1500ul) << "Watermark is 1500 at start";
		EXPECT_EQ(uxTaskGetStackHighWaterMark(handle), 1564ul) << "Watermark is 1564 after second call";
		EXPECT_EQ(uxTaskGetStackHighWaterMark(handle), 1628ul) << "Watermark is 1628 after third call";
		EXPECT_EQ(uxTaskGetStackHighWaterMark(handle), 1628ul) << "Watermark is 1628 after fourth call";
		EXPECT_EQ(uxTaskGetStackHighWaterMark(handle), 1628ul) << "Watermark is 1628 after fifth call";
		EXPECT_EQ(uxTaskGetStackHighWaterMark(handle), 1500ul) << "Watermark is 1500 after sixth call";

		TaskHandle_t handle2;
		EXPECT_EQ(pdTRUE, xTaskCreatePinnedToCore([](void*) {}, "task1", 1000, nullptr, 1, &handle2, 1)) << "Task 1 created";
		EXPECT_EQ(uxTaskGetStackHighWaterMark(handle2), 3750ul) << "Watermark is 3750 for second handle";
		EXPECT_EQ(uxTaskGetStackHighWaterMark(handle2), 3750ul) << "Watermark stays 3750 for second handle";

		TaskHandle_t handle3;
		EXPECT_EQ(pdTRUE, xTaskCreatePinnedToCore([](void*) {}, "task2", 1000, nullptr, 1, &handle3, 1)) << "Task 2 created";
		EXPECT_EQ(uxTaskGetStackHighWaterMark(handle3), 5250ul) << "Watermark is 5250 for third handle";
		EXPECT_EQ(uxTaskGetStackHighWaterMark(handle3), 5250ul) << "Watermark stays 5250 for third handle";
	}

	TEST_F(FreeRtosTest, TaskNotifyTest) {
		TaskHandle_t handle = nullptr;
		EXPECT_EQ(pdTRUE, xTaskCreatePinnedToCore([](void*) {}, "task1", 1000, nullptr, 1, &handle, 1)) << "Task 1 created";
		EXPECT_EQ(ulTaskNotifyTake(pdTRUE, portMAX_DELAY), 0u) << "Take returns 0 before give";
		vTaskNotifyGiveFromISR(handle, nullptr);
		EXPECT_EQ(ulTaskNotifyTake(pdTRUE, portMAX_DELAY), 1u) << "Take returns 1 after give";
	}

	TEST_F(FreeRtosTest, QueueOverrunTest) {
		testUxQueueReset();
		const auto handle = xQueueCreate(kMaxQueues, 2);
		for (int i = 0; i < 20; i++) {
			EXPECT_EQ(xQueueSendToBack(handle, buffer, 0), pdTRUE) << "Item " << i << " sent";
		}
		EXPECT_EQ(xQueueSendToBack(handle, buffer, 0), pdFALSE) << "Item 21 fails";
		testUxQueueReset();
	}
}
