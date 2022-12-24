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

namespace TL
{
	class WiFiConfigurationEndpoint : public RestEndpoint
	{
	public:
		static void begin(TL::Configuration *_configuration, std::function<bool()> _configChangedCallback);

	private:
		WiFiConfigurationEndpoint();

		static TL::Configuration *configuration;
		static std::function<bool()> configChangedCallback;

		static void getWiFiConfig();
		static void postWiFiConfig();

		static bool validateConfiguration(const JsonObject &jsonObject);
		static bool isInRange(const long value, const long min, const long max);
		static bool validateWiFiSsid(const String ssid);
		static bool validateWiFiPassword(const String password);
	};
}

#endif
