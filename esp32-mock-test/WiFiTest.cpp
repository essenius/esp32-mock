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
#include "../esp32-mock/WiFi.h"

namespace Esp32MockTest {
	TEST(WifiTest, InitTest) {
		EXPECT_STREQ("00:11:22:33:44:55", WiFi.macAddress().c_str());
		for (int i = 0; i < 5; i++) {
			EXPECT_FALSE(WiFi.isConnected());
		}
		EXPECT_TRUE(WiFi.isConnected());
		EXPECT_TRUE(WiFi.isConnected());
		EXPECT_EQ(13, WiFi.channel());
		EXPECT_STREQ("55:44:33:22:11:00", WiFi.BSSIDstr().c_str());
		EXPECT_STREQ("esp32_001122334455", WiFi.getHostname());
		EXPECT_STREQ("", WiFi.SSID().c_str());
		EXPECT_STREQ("192.168.1.0", WiFi.networkID().toString().c_str());
		EXPECT_STREQ("10.0.0.2", WiFi.localIP().toString().c_str());
		EXPECT_STREQ("10.0.0.1", WiFi.gatewayIP().toString().c_str());
		EXPECT_STREQ("255.255.0.0", WiFi.subnetMask().toString().c_str());
		EXPECT_STREQ("8.8.8.8", WiFi.dnsIP().toString().c_str());
		EXPECT_STREQ("8.8.4.4", WiFi.dnsIP(1).toString().c_str());

		WiFi.begin("ssid", "password");
		EXPECT_STREQ("ssid", WiFi.SSID().c_str());

		EXPECT_TRUE(WiFi.setHostname("hostname"));
		EXPECT_STREQ("hostname", WiFi.getHostname());
		EXPECT_FALSE(WiFi.setHostname(""));
		EXPECT_STREQ("hostname", WiFi.getHostname());
	}

	TEST(WifiTest, ConnectInTest) {
		WiFi.connectIn(2);
		EXPECT_FALSE(WiFi.isConnected());
		EXPECT_FALSE(WiFi.isConnected());
		EXPECT_TRUE(WiFi.isConnected());
	}

	TEST(WifiTest, ConfigTest) {
		WiFi.config(IPAddress(1, 2, 3, 4), IPAddress(5, 6, 7, 8), IPAddress(9, 10, 11, 12), IPAddress(13,14,15,16), IPAddress(17,18,19,20));
		EXPECT_STREQ("1.2.3.4", WiFi.localIP().toString().c_str());
		EXPECT_STREQ("5.6.7.8", WiFi.gatewayIP().toString().c_str());
		EXPECT_STREQ("9.10.11.12", WiFi.subnetMask().toString().c_str());
		EXPECT_STREQ("13.14.15.16", WiFi.dnsIP(0).toString().c_str());
		EXPECT_STREQ("17.18.19.20", WiFi.dnsIP(1).toString().c_str());

		WiFi.config(IPAddress(1, 2, 3, 4), IPAddress(5, 6, 7, 8), IPAddress(9, 10, 11, 12), IPAddress(13, 14, 15, 16));
		EXPECT_STREQ("13.14.15.16", WiFi.dnsIP(1).toString().c_str());

		WiFi.config(IPAddress(1, 2, 3, 4), IPAddress(5, 6, 7, 8), IPAddress(9, 10, 11, 12));
		EXPECT_STREQ("5.6.7.8", WiFi.dnsIP(0).toString().c_str());
		EXPECT_STREQ("5.6.7.8", WiFi.dnsIP(1).toString().c_str());
	}
}