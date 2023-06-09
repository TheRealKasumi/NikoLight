/**
 * @file WiFiManager.h
 * @author TheRealKasumi
 * @brief Contains a simple WiFi manager.
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
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <stdint.h>
#include <WiFi.h>

namespace NL
{
	class WiFiManager
	{
	public:
		enum class Error
		{
			OK,					  // No error
			ERROR_SET_MODE,		  // Failed to set WiFi mode
			ERROR_START_AP,		  // Failed to start access point
			ERROR_INVALID_SSID,	  // Invalid ssid
			ERROR_INVALID_PW,	  // Invalid password
			ERROR_CONNECT_FAILED, // Failed to connect to a WiFi network
			ERROR_CONNECT_TIMEOUT // Failed to connect to a WiFi network due to timeout
		};

		static NL::WiFiManager::Error begin();
		static NL::WiFiManager::Error end();
		static bool isInitialized();

		static NL::WiFiManager::Error startAccessPoint(const char *ssid, const char *password = 0, uint8_t channel = 1, bool hidden = 0, uint8_t maxConnections = 1);
		static NL::WiFiManager::Error connectTo(const char *ssid, const char *password, const uint32_t timeout);

	private:
		WiFiManager();

		static bool initialized;
	};
}

#endif
