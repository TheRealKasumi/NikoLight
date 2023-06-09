/**
 * @file WiFiConfigurationEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to configure the WiFi settings.
 *
 * @copyright Copyright (c) 2023 TheRealKasumi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef WIFI_CONFIGURATION_ENDPOINT_H
#define WIFI_CONFIGURATION_ENDPOINT_H

#include "server/RestEndpoint.h"
#include "configuration/Configuration.h"
#include "configuration/SystemConfiguration.h"
#include "wifi/WiFiManager.h"
#include "logging/Logger.h"

namespace NL
{
	class WiFiConfigurationEndpoint : public RestEndpoint
	{
	public:
		static void begin();

	private:
		WiFiConfigurationEndpoint();

		static void getWiFiConfig();
		static void patchWiFiConfig();

		static bool validateConfiguration(const JsonObject &jsonObject);
		static bool isInRange(const long value, const long min, const long max);
		static bool validateWiFiSsid(const String ssid);
		static bool validateWiFiPassword(const String password);
		static bool hasChanged(const NL::Configuration::WiFiConfig &newConfig);
	};
}

#endif
