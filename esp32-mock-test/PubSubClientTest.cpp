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
#include "../esp32-mock/PubSubClient.h"
#include <SafeCString.h>

namespace Esp32MockTest {
	class PubSubClientTest : public testing::Test {
	public:

		void callback(const char* topic, const uint8_t* payload, const unsigned int length) {
			_callCount++;
			_payloadOverflow = length >= BufferSize;
			SafeCString::sprintf(_topic, "%s", topic);
			_topicOverflow = length >= BufferSize;
			const auto usedLength = std::min(length, BufferSize - 1);
			for (unsigned int i = 0; i < usedLength; i++) {
				_payload[i] = payload[i];
			}
			_payload[usedLength] = '\0';
		}

		const char* getTopic() const { return _topic; }
		const char* getPayload() const { return _payload; }
		bool hasTopicOverflow() const { return _topicOverflow; }
		bool hasPayloadOverflow() const { return _payloadOverflow; }

	private:
		constexpr static unsigned int BufferSize = 100;
		unsigned int _callCount = 0;
		bool _topicOverflow = false;
		bool _payloadOverflow = false;
		char _payload[BufferSize] = "";
		char _topic[BufferSize] = "";
	};

	TEST_F(PubSubClientTest, InitTest) {
		PubSubClient client;
		EXPECT_EQ(3, client.state());
		EXPECT_EQ(0, client.getCallCount());
		EXPECT_EQ(0, client.getLoopCount());
		EXPECT_STREQ("", client.getPayloads());
		EXPECT_STREQ("", client.getTopics());

		EXPECT_TRUE(client.connect("testid", "user", "pass", "will", 0, false, "lost"));
		EXPECT_STREQ("testid", client.id());
		EXPECT_STREQ("user", client.user());
		EXPECT_TRUE(client.connected());
		EXPECT_TRUE(client.unsubscribe("dummy"));
		EXPECT_TRUE(client.subscribe("dummy"));
		client.setCanSubscribe(false);
		EXPECT_FALSE(client.subscribe("dummy2"));
		client.setCallback([=](const char* topic, const uint8_t* payload, const unsigned int length) {
			this->callback(topic, payload, length);
		});

		client.loop();
		EXPECT_EQ(0, client.getCallCount());
		EXPECT_EQ(1, client.getLoopCount());

		client.setLoopCallback("topic", reinterpret_cast<const uint8_t*>("payload"), 7);
		client.loop();
		EXPECT_EQ(0, client.getCallCount());
		EXPECT_EQ(2, client.getLoopCount());
		EXPECT_STREQ("topic", getTopic());
		EXPECT_STREQ("payload", getPayload());
		EXPECT_FALSE(hasTopicOverflow());
		EXPECT_FALSE(hasPayloadOverflow());
		client.setLoopCallback("ABCDEFGHIJabcdefghijABCDEFGHIJabcdefghijABCDEFGHIJabcdefghijABCDEFGHIJabcdefghijABCDEFGHIJabcdefghij", reinterpret_cast<const uint8_t*>("123456789012345678901234567890"), 20);
		client.loop();
		EXPECT_EQ(0, client.getCallCount());
		EXPECT_EQ(3, client.getLoopCount());
		EXPECT_STREQ("ABCDEFGHIJabcdefghijABCDEFGHIJabcdefghijABCDEFGHIJabcdefghijABCDEFGHIJabcdefghijABCDEFGHIJabcdefghi", getTopic());
		EXPECT_STREQ("123456789", getPayload());
		// the overflow should not reach the callback
		EXPECT_FALSE(hasTopicOverflow());
		EXPECT_FALSE(hasPayloadOverflow());

		EXPECT_TRUE(client.publish("topic", "payload"));
		EXPECT_EQ(1, client.getCallCount());
		EXPECT_STREQ("topic\n", client.getTopics());
		EXPECT_STREQ("payload[x]\n", client.getPayloads());

		client.setCanPublish(false);
		EXPECT_FALSE(client.publish("topic", "payload"));

		client.reset();
		EXPECT_STREQ("", client.id());
		EXPECT_STREQ("", client.user());

		EXPECT_TRUE(client.publish("topic", "payload", true));
		EXPECT_EQ(1, client.getCallCount());
		EXPECT_STREQ("topic\n", client.getTopics());
		EXPECT_STREQ("payload\n", client.getPayloads());

		client.reset();

		for (int i = 0; i < 25; i++) {
			EXPECT_TRUE(client.publish("ABCDEFGHIJabcdefghijABCDEFGHIJabcdefghijABCDEFGHIJabcdefghijABCDEFGHIJabcdefghijABCDEFGHIJabcdefghi", "123456789")) << "#" << i;
		}
		EXPECT_FALSE(client.publish("ABCDEFGHIJabcdefghijABCDEFGHIJabcdefghijABCDEFGHIJabcdefghijABCDEFGHIJabcdefghijABCDEFGHIJabcdefghi", "123456789"));
		for (int i = 0; i < 54; i++) {
			EXPECT_TRUE(client.publish("", "123456789")) << "#" << i;
		}
		EXPECT_FALSE(client.publish("", "123456789"));
	}
}