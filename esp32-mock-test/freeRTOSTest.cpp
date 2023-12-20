// Copyright 2023 Rik Essenius
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

namespace Esp32MockTest {
    class FreeRtosTest : public testing::Test {
    public:
        QueueHandle_t queue[MaxQueues] = { nullptr };
        char buffer[10] = "x";
    };

	TEST_F(FreeRtosTest, QueueTest) {
		uxQueueReset();
        for (auto& i : queue) {
            i = xQueueCreate(MaxQueues, 2);
            EXPECT_NE(nullptr, i) << "Not null";
        }
        EXPECT_EQ(nullptr, xQueueCreate(MaxQueues, 2)) << "Next returns nullptr";
        EXPECT_EQ(20ul, uxQueueSpacesAvailable(queue[0])) << "queue 0 has 20 spaces left at start";
        EXPECT_EQ(0ul, uxQueueMessagesWaiting(queue[0])) << "queue 0 has 0 waiting message at start";
        EXPECT_EQ(pdFALSE, xQueueReceive(queue[0], buffer, 0)) << "Nothing in the queue";
        EXPECT_EQ(pdFALSE, xQueueSendToFront(queue[0], buffer, 0)) << "Not implemented";
        EXPECT_EQ(pdTRUE, xQueueSendToBack(queue[0], buffer, 0)) << "Item 1 sent";
        EXPECT_EQ(19ul, uxQueueSpacesAvailable(queue[0])) << "queue 0 has 19 spaces left";
        EXPECT_EQ(1ul, uxQueueMessagesWaiting(queue[0])) << "queue 0 has 1 waiting message";
        buffer[0] = 'y';
        EXPECT_EQ(pdTRUE, xQueueSendToBack(queue[0], buffer, 0)) << "Item 2 sent";
        EXPECT_EQ(2ul, uxQueueMessagesWaiting(queue[0])) << "queue 0 has 2 waiting message";

        buffer[0] = '\0';
        EXPECT_EQ(pdTRUE, xQueueReceive(queue[0], buffer, 0)) << "Item read";
        EXPECT_STREQ("x", buffer) << "read item has right value";
        EXPECT_EQ(1ul, uxQueueMessagesWaiting(queue[0])) << "queue 0 has 1 waiting message after read";
        buffer[0] = '\0';
        EXPECT_EQ(pdTRUE, xQueueReceive(queue[0], buffer, 0)) << "Item read";
        EXPECT_STREQ("y", buffer) << "read item has right value";
        EXPECT_EQ(20ul, uxQueueSpacesAvailable(queue[0])) << "queue 0 has 20 spaces left after reads";
        EXPECT_EQ(0ul, uxQueueMessagesWaiting(queue[0])) << "queue 0 has 0 waiting message after read";

        EXPECT_EQ(pdTRUE, xQueueSendToBack(queue[1], buffer, 0)) << "Item sent to queue 1";
        EXPECT_EQ(pdTRUE, xQueueSendToBack(queue[3], buffer, 0)) << "Item sent to queue 3";
        uxQueueReset();
        EXPECT_NE(nullptr, xQueueCreate(5, 2)) << "Can create new queue after reset";
        uxQueueReset();
	}

    TEST_F(FreeRtosTest, WatermarkTest) {
        uxTaskGetStackHighWaterMarkReset();
        const auto handle = xTaskGetCurrentTaskHandle();
        EXPECT_EQ(1500ul, uxTaskGetStackHighWaterMark(handle)) << "Watermark is 1500 at start";
        EXPECT_EQ(1564ul, uxTaskGetStackHighWaterMark(handle)) << "Watermark is 1564 after second call";
        EXPECT_EQ(1628ul, uxTaskGetStackHighWaterMark(handle)) << "Watermark is 1628 after third call";
        EXPECT_EQ(1628ul, uxTaskGetStackHighWaterMark(handle)) << "Watermark is 1628 after fourth call";
        EXPECT_EQ(1628ul, uxTaskGetStackHighWaterMark(handle)) << "Watermark is 1628 after fifth call";
        EXPECT_EQ(1500ul, uxTaskGetStackHighWaterMark(handle)) << "Watermark is 1500 after sixth call";

        TaskHandle_t handle2;
        EXPECT_EQ(pdTRUE, xTaskCreatePinnedToCore([](void*) {}, "task1", 1000, nullptr, 1, &handle2, 1)) << "Task 1 created";
        EXPECT_EQ(3750ul, uxTaskGetStackHighWaterMark(handle2)) << "Watermark is 3750 for second handle";
        EXPECT_EQ(3750ul, uxTaskGetStackHighWaterMark(handle2)) << "Watermark stays 3750 for second handle";

        TaskHandle_t handle3;
        EXPECT_EQ(pdTRUE, xTaskCreatePinnedToCore([](void*) {}, "task2", 1000, nullptr, 1, &handle3, 1)) << "Task 2 created";
        EXPECT_EQ(5250ul, uxTaskGetStackHighWaterMark(handle3)) << "Watermark is 5250 for third handle";
        EXPECT_EQ(5250ul, uxTaskGetStackHighWaterMark(handle3)) << "Watermark stays 5250 for third handle";
    }

    TEST_F(FreeRtosTest, TaskNotifyTest) {
		TaskHandle_t handle = nullptr;
		EXPECT_EQ(pdTRUE, xTaskCreatePinnedToCore([](void*) {}, "task1", 1000, nullptr, 1, &handle, 1)) << "Task 1 created";
		EXPECT_EQ(0u, ulTaskNotifyTake(pdTRUE, portMAX_DELAY)) << "Take returns 0 before give";
        vTaskNotifyGiveFromISR(handle, nullptr);
        EXPECT_EQ(1u, ulTaskNotifyTake(pdTRUE, portMAX_DELAY)) << "Take returns 1 after give";
	}
}