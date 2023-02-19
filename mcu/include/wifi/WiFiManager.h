/**
 * @file WiFiManager.h
 * @author TheRealKasumi
 * @brief Contains a simple WiFi manager.
 *
 * @copyright Copyright (c) 2022-2023 TheRealKasumi
 *
 * This project, including hardware and software, is provided "as is". There is no warranty
 * of any kind, express or implied, including but not limited to the warranties of fitness
 * for a particular purpose and noninfringement. TheRealKasumi (https://github.com/TheRealKasumi)
 * is holding ownership of this project. You are free to use, modify, distribute and contribute
 * to this project for private, non-commercial purposes. It is granted to include this hardware
 * and software into private, non-commercial projects. However, the source code of any project,
 * software and hardware that is including this project must be public and free to use for private
 * persons. Any commercial use is hereby strictly prohibited without agreement from the owner.
 * By contributing to the project, you agree that the ownership of your work is transferred to
 * the project owner and that you lose any claim to your contribute work. This copyright and
 * license note applies to all files of this project and must not be removed without agreement
 * from the owner.
 *
 */
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <stdint.h>
#include <WiFi.h>

namespace TL
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

		static TL::WiFiManager::Error begin();
		static TL::WiFiManager::Error end();
		static bool isInitialized();

		static TL::WiFiManager::Error startAccessPoint(const char *ssid, const char *password = 0, uint8_t channel = 1, bool hidden = 0, uint8_t maxConnections = 1);
		static TL::WiFiManager::Error connectTo(const char *ssid, const char *password, const uint32_t timeout);

	private:
		WiFiManager();

		static bool initialized;
	};
}

#endif
