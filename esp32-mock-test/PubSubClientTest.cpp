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
#include "../esp32-mock/PubSubClient.h"
#include <SafeCString.h>

namespace esp32_mock_test {
	constexpr auto kTopicWithMaxSize = R"(ABCDEFGHIJabcdefghijABCDEFGHIJabcdefghijABCDEFGHIJabcdefghijABCDEFGHIJabcdefghijABCDEFGHIJabcdefghi)";
	constexpr auto kTooLargeTopic =  R"(ABCDEFGHIJabcdefghijABCDEFGHIJabcdefghijABCDEFGHIJabcdefghijABCDEFGHIJabcdefghijABCDEFGHIJabcdefghij)";
	constexpr auto kPayloadWithMaxSize = R"(1234567890123456789)";
	constexpr auto kTooLargePayload = R"(123456789012345678901234567890)";

	class PubSubClientTest : public testing::Test {
	public:

		void callback(const char* topic, const uint8_t* payload, const unsigned int length) {
			_callCount++;
			_payloadOverflow = length >= kBufferSize;
			SafeCString::sprintf(_topic, "%s", topic);
			_topicOverflow = length >= kBufferSize;
			const auto usedLength = std::min(length, kBufferSize - 1);
			for (unsigned int i = 0; i < usedLength; i++) {
				_payload[i] = static_cast<char>(payload[i]);
			}
			_payload[usedLength] = '\0';
		}

		static void assertCall(const PubSubClient& client, const int callCount, const char* topic, const char* payload) {
			ASSERT_EQ(callCount, client.testGetCallCount()) << "CallCount OK";
			ASSERT_STREQ(topic, client.testGetTopics()) << "Topics OK";
			ASSERT_STREQ(payload, client.testGetPayloads()) << "Payloads OK";
		}

		const char* getTopic() const { return _topic; }
		const char* getPayload() const { return _payload; }
		bool hasTopicOverflow() const { return _topicOverflow; }
		bool hasPayloadOverflow() const { return _payloadOverflow; }

	private:
		constexpr static unsigned int kBufferSize = 100;
		unsigned int _callCount = 0;
		bool _topicOverflow = false;
		bool _payloadOverflow = false;
		char _payload[kBufferSize] = "";
		char _topic[kBufferSize] = "";
	};

	TEST_F(PubSubClientTest, FailPublishAfterTest) {
		PubSubClient client;
		client.testFailPublishAfter(3);
		EXPECT_TRUE(client.connect("testid", "user", "pass", "will", 0, false, "lost"))  << "Connect succeeds";
		for (auto i = 0; i < 3; i++) {
			EXPECT_TRUE(client.publish("topic", "payload")) << "Publish #" << i << " succeeds ";
		}
		EXPECT_FALSE(client.publish("topic", "payload")) << "Publish #4 fails";
	}

	TEST_F(PubSubClientTest, SubscribeTest) {
		PubSubClient client;
		EXPECT_EQ(client.state(), 3) << "State is 3 at the start";
		EXPECT_EQ(client.testGetLoopCount(), 0) << "Loop count is 0 at the start";
		EXPECT_FALSE(client.connected()) << "Not connected at the start";

		assertCall(client, 0, "", "");
		EXPECT_TRUE(client.connect("testid", "user", "pass", "will", 0, false, "lost")) << "Connect succeeds";
		EXPECT_STREQ(client.testGetId(), "testid") << "id is testid";
		EXPECT_STREQ(client.testGetUser(), "user") << "user is user";
		EXPECT_TRUE(client.unsubscribe("dummy")) << "unsubscribe on nonexisting topic works";
		EXPECT_TRUE(client.subscribe("dummy")) << "subscribe to dummy works";
		client.testSetCanSubscribe(false);
		EXPECT_FALSE(client.subscribe("dummy2")) << "subscribe after setCanSubscribe(false) does not work";
	}

	TEST_F(PubSubClientTest, CanConnectTest) {
		PubSubClient client;
		client.testSetCanConnect(false);
		EXPECT_FALSE(client.connected()) << "Not connected after setCanConnect (false) and connected not called";
		assertCall(client, 0, "", "");
		EXPECT_FALSE(client.connect("testid", "user", "pass", "will", 0, false, "lost")) << "connect fails after setCanConnect(false)";
		EXPECT_FALSE(client.connected()) << "Not connected after failed connect";
		client.testSetCanConnect(true);
		EXPECT_FALSE(client.connected()) << "Not connected after failed connect but setCanConnect(true)";
		EXPECT_TRUE(client.connect("testid", "user", "pass", "will", 0, false, "lost")) << "connect succeeds after setCanConnect(true)";
		EXPECT_TRUE(client.connected()) << "connected after succeeded connect";
	}

	TEST_F(PubSubClientTest, LoopTest) {

		PubSubClient client;
		EXPECT_TRUE(client.connect("testid", "user", "pass", "will", 0, false, "lost")) << "connect succeeds";
		client.setCallback([this](const char* topic, const uint8_t* payload, const unsigned int length) {
			this->callback(topic, payload, length);
		});


		client.loop();
		EXPECT_EQ(client.testGetCallCount(), 0) << "CallCount is 0 after first loop";
		EXPECT_EQ(client.testGetLoopCount(), 1) << "LoopCount is 1 after first loop";

		client.setLoopCallback("topic", reinterpret_cast<const uint8_t*>("payload"), 7);
		client.loop();
		EXPECT_EQ(client.testGetCallCount(), 0) << "CallCount is 0 after second loop";
		EXPECT_EQ(client.testGetLoopCount(), 2) << "LoopCount is 2 after first loop";
		EXPECT_STREQ(getTopic(), "topic") << "Topic was picked up by callback";
		EXPECT_STREQ(getPayload(), "payload") << "Payload was picked up by callback";
		EXPECT_FALSE(hasTopicOverflow()) << "There is no topic overflow";
		EXPECT_FALSE(hasPayloadOverflow()) << "There is no payload overflow";
		client.setLoopCallback(kTooLargeTopic, reinterpret_cast<const uint8_t*>(kTooLargePayload), 20);
		client.loop();
		EXPECT_EQ(client.testGetCallCount(), 0) << "CallCount is still 0 after third loop";
		EXPECT_EQ(client.testGetLoopCount(), 3) << "LoopCount is 3 after third loop";
		EXPECT_STREQ(getTopic(), kTopicWithMaxSize) << "topic was picked up but cut off";
		EXPECT_STREQ(getPayload(), kPayloadWithMaxSize) << "payload was picked up but cut off";

		EXPECT_FALSE(hasTopicOverflow()) << "The topic buffer overflow should not reach the callback";
		EXPECT_FALSE(hasPayloadOverflow()) << "The payload buffer overflow should not reach the callback";
	}

	TEST_F(PubSubClientTest, PublishTest) {
		PubSubClient client;
		EXPECT_TRUE(client.connect("testid", "user", "pass", "will", 0, false, "lost")) << "Connect succeeds";
		EXPECT_TRUE(client.publish("topic", "payload")) << "Publish works";

		assertCall(client, 1, "topic\n", "payload[x]\n");

		client.testSetCanPublish(false);
		EXPECT_FALSE(client.publish("topic", "payload")) << "Publish fails after setCanPublish(false)";

		client.testReset();
		EXPECT_STREQ(client.testGetId(), "") << "id is empty string after reset";
		EXPECT_STREQ(client.testGetUser(), "") << "user is empty string after reset";

		EXPECT_TRUE(client.publish("topic", "payload", true)) << "publish works after reset";
		assertCall(client, 1, "topic\n", "payload\n");

		client.testReset();

		for (int i = 0; i < 25; i++) {
			EXPECT_TRUE(client.publish(kTopicWithMaxSize, "123456789")) << "Publish round 1 succeeds #" << i;
		}
		EXPECT_FALSE(client.publish(kTopicWithMaxSize, "123456789")) << "Publish round 1 fails due to topic buffer overflow";
		for (int i = 0; i < 54; i++) {
			EXPECT_TRUE(client.publish("", "123456789")) << "Publish round 2 succeeds #" << i;
		}
		EXPECT_FALSE(client.publish("", "123456789")) << "Publish round 2 fails due to payload buffer overflow";
	}
}