/**
 * @file WiFiConfigurationEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to configure the WiFi settings.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef WIFI_CONFIGURATION_ENDPOINT_H
#define WIFI_CONFIGURATION_ENDPOINT_H

#include <functional>

#include "server/RestEndpoint.h"
#include "configuration/Configuration.h"
#include "configuration/SystemConfiguration.h"
#include "logging/Logger.h"
#include "util/InMemoryBinaryFile.h"

extern "C"
{
#include "crypto/base64.h"
}

namespace TesLight
{
	class WiFiConfigurationEndpoint : public RestEndpoint
	{
	public:
		static void begin(TesLight::Configuration *_configuration, std::function<void()> _configChangedCallback);

	private:
		WiFiConfigurationEndpoint();

		static TesLight::Configuration *configuration;
		static std::function<void()> configChangedCallback;

		static void getWiFiConfig(AsyncWebServerRequest *request);
		static void postWiFiConfig(AsyncWebServerRequest *request);

		static bool validateWiFiSsid(const String ssid);
		static bool validateWiFiPassword(const String password);
		static bool validateWiFiChannel(const int channel);
		static bool validateWiFiMaxConnections(const int maxConnections);
		static bool hasValidSsidChars(const String input);
		static bool hasValidPasswordChars(const String input);
	};
}

#endif
