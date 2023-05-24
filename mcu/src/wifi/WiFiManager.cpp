/**
 * @file WiFiManager.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link NL::WiFiManager}.
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
#include "wifi/WiFiManager.h"

bool NL::WiFiManager::initialized = false;

/**
 * @brief Start the WiFi manager.
 * @return OK when WiFi was initialized
 * @return ERROR_SET_MODE when the WiFi mode could not be set
 */
NL::WiFiManager::Error NL::WiFiManager::begin()
{
	NL::WiFiManager::initialized = false;
	WiFi.persistent(false);
	if (!WiFi.mode(WIFI_MODE_AP))
	{
		return NL::WiFiManager::Error::ERROR_SET_MODE;
	}
	NL::WiFiManager::initialized = true;
	return NL::WiFiManager::Error::OK;
}

/**
 * @brief Stop the WiFi manager.
 * @return OK when WiFi was stopped
 * @return ERROR_SET_MODE when the WiFi mode could not be set
 */
NL::WiFiManager::Error NL::WiFiManager::end()
{
	NL::WiFiManager::initialized = false;
	if (!WiFi.mode(WIFI_MODE_NULL))
	{
		return NL::WiFiManager::Error::ERROR_SET_MODE;
	}
	return NL::WiFiManager::Error::OK;
}

/**
 * @brief Check if the WiFi manager is initialized
 * @return true when initialized
 * @return false when not initialized
 */
bool NL::WiFiManager::isInitialized()
{
	return NL::WiFiManager::initialized;
}

/**
 * @brief Start the access point for clients to connect to.
 * @param ssid ssid of the ap
 * @param password password of the ap
 * @param channel channel of the ap
 * @param hidden make the ap hidden/visible
 * @param maxConnections maximum number of connections
 * @return OK when the AP was started
 * @return ERROR_START_AP when the AP could not be started
 * @return ERROR_INVALID_SSID when the AP ssid is invalid
 * @return ERROR_INVALID_PW when the AP password is invalid
 */
NL::WiFiManager::Error NL::WiFiManager::startAccessPoint(const char *ssid, const char *password, uint8_t channel, bool hidden, uint8_t maxConnections)
{
	if (strlen(ssid) < 4)
	{
		return NL::WiFiManager::Error::ERROR_INVALID_SSID;
	}
	else if (strlen(password) > 0 && strlen(password) < 8)
	{
		return NL::WiFiManager::Error::ERROR_INVALID_PW;
	}

	if (!WiFi.softAP(ssid, strlen(password) > 0 ? password : nullptr, channel, hidden, maxConnections))
	{
		return NL::WiFiManager::Error::ERROR_START_AP;
	}
	delay(100);

	IPAddress ip(192, 168, 4, 1);
	IPAddress nMask(255, 255, 255, 0);
	WiFi.softAPConfig(ip, ip, nMask);

	return NL::WiFiManager::Error::OK;
}

/**
 * @brief Connect to a WiFi network.
 * @param ssid ssid of the WiFi network
 * @param password password of the WiFi network
 * @param timeout connection timeout
 * @return OK when the controller was connected to the WiFi network
 * @return ERROR_INVALID_SSID when the WiFi ssid is invalid
 * @return ERROR_INVALID_PW when the WiFi password is invalid
 * @return ERROR_CONNECT_FAILED when the connection failed
 * @return ERROR_CONNECT_TIMEOUT when the connection timed out
 */
NL::WiFiManager::Error NL::WiFiManager::connectTo(const char *ssid, const char *password, const uint32_t timeout)
{
	if (strlen(ssid) < 4)
	{
		return NL::WiFiManager::Error::ERROR_INVALID_SSID;
	}
	else if (strlen(password) > 0 && strlen(password) < 8)
	{
		return NL::WiFiManager::Error::ERROR_INVALID_PW;
	}

	const wl_status_t connError = WiFi.begin(ssid, password);
	if (connError == WL_CONNECT_FAILED)
	{
		return NL::WiFiManager::Error::ERROR_CONNECT_FAILED;
	}
	WiFi.setAutoReconnect(true);

	uint64_t start = millis();
	while (WiFi.status() != WL_CONNECTED)
	{
		if (millis() - start > timeout)
		{
			return NL::WiFiManager::Error::ERROR_CONNECT_TIMEOUT;
		}
	}

	return NL::WiFiManager::Error::OK;
}
