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
#include "../esp32-mock/WiFi.h"

namespace esp32_mock_test {
	TEST(WifiTest, InitTest) {
		WiFi.testReset();
		EXPECT_STREQ(WiFi.macAddress().c_str(), "00:11:22:33:44:55");
		for (int i = 0; i < 5; i++) {
			EXPECT_FALSE(WiFi.isConnected()) << "Not connected #" << i;
		}
		EXPECT_TRUE(WiFi.isConnected()) << "Connected after 5 times";
		EXPECT_TRUE(WiFi.isConnected()) << "Stays connected";
		EXPECT_EQ(WiFi.channel(), 13);
		EXPECT_STREQ(WiFi.BSSIDstr().c_str(), "55:44:33:22:11:00");
		EXPECT_STREQ(WiFi.getHostname(), "esp32_001122334455");
		EXPECT_STREQ(WiFi.SSID().c_str(), "");
		EXPECT_STREQ(WiFi.networkID().toString().c_str(), "192.168.1.0");
		EXPECT_STREQ(WiFi.localIP().toString().c_str(), "10.0.0.2");
		EXPECT_STREQ(WiFi.gatewayIP().toString().c_str(), "10.0.0.1");
		EXPECT_STREQ(WiFi.subnetMask().toString().c_str(), "255.255.0.0");
		EXPECT_STREQ(WiFi.dnsIP().toString().c_str(), "8.8.8.8");
		EXPECT_STREQ(WiFi.dnsIP(1).toString().c_str(), "8.8.4.4");

		WiFi.begin("ssid", "password");
		EXPECT_STREQ(WiFi.SSID().c_str(), "ssid");

		EXPECT_TRUE(WiFi.setHostname("hostname"));
		EXPECT_STREQ(WiFi.getHostname(), "hostname");
		EXPECT_FALSE(WiFi.setHostname("")) << "Setting hostname to empty string fails";
		EXPECT_STREQ(WiFi.getHostname(), "hostname") << "Setting hostname to empty string did not succeed";
	}

	TEST(WifiTest, ConnectInTest) {
		WiFi.testConnectIn(2);
		EXPECT_FALSE(WiFi.isConnected()) << "First time disconnected";
		EXPECT_FALSE(WiFi.isConnected()) << "Second time disconnected";
		EXPECT_TRUE(WiFi.isConnected()) << "Third time connected";
	}

	TEST(WifiTest, ConfigTest) {
		WiFi.config(IPAddress(1, 2, 3, 4), IPAddress(5, 6, 7, 8), IPAddress(9, 10, 11, 12), IPAddress(13, 14, 15, 16), IPAddress(17, 18, 19, 20));
		EXPECT_STREQ(WiFi.localIP().toString().c_str(), "1.2.3.4");
		EXPECT_STREQ(WiFi.gatewayIP().toString().c_str(), "5.6.7.8");
		EXPECT_STREQ(WiFi.subnetMask().toString().c_str(), "9.10.11.12");
		EXPECT_STREQ(WiFi.dnsIP(0).toString().c_str(), "13.14.15.16");
		EXPECT_STREQ(WiFi.dnsIP(1).toString().c_str(), "17.18.19.20");

		WiFi.config(IPAddress(1, 2, 3, 4), IPAddress(5, 6, 7, 8), IPAddress(9, 10, 11, 12), IPAddress(13, 14, 15, 16));
		EXPECT_STREQ(WiFi.dnsIP(1).toString().c_str(), "13.14.15.16");

		WiFi.config(IPAddress(1, 2, 3, 4), IPAddress(5, 6, 7, 8), IPAddress(9, 10, 11, 12));
		EXPECT_STREQ(WiFi.dnsIP(0).toString().c_str(), "5.6.7.8");
		EXPECT_STREQ(WiFi.dnsIP(1).toString().c_str(), "5.6.7.8");
	}
}
