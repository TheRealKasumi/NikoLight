/**
 * @file WiFiManager.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TL::WiFiManager}.
 *
 * @copyright Copyright (c) 2022 TheRealKasumi
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

/**
 * @brief Create a new instance of {@link TL::WiFiManager}.
 */
TL::WiFiManager::WiFiManager()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initializing WiFi manager."));
	WiFi.mode(WIFI_MODE_APSTA);
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi manager initialized."));
}

/**
 * @brief Destroy the {@link TL::WiFiManager} instance.
 */
TL::WiFiManager::~WiFiManager()
{
}

/**
 * @brief Start the access point for clients to connect to.
 * @param ssid ssid of the ap
 * @param password password of the ap
 * @param channel channel of the ap
 * @param hidden make the ap hidden/visible
 * @param maxConnections maximum number of connections
 * @return true when successful
 * @return false when there was an error
 */
bool TL::WiFiManager::startAccessPoint(const char *ssid, const char *password, uint8_t channel, bool hidden, uint8_t maxConnections)
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Starting WiFi access point with ssid '") + String(ssid) + F("' and password '") + String(password) + F("' on channel ") + channel + F("."));

	if (hidden)
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi access point is configured to start in hidden mode."));
	}

	if (!WiFi.softAP(ssid, strlen(password) > 0 ? password : nullptr, channel, hidden, maxConnections))
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi access point could not be started."));
		return false;
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Starting WiFi access point."));
	delay(100);

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Configuring WiFi access point."));
	IPAddress ip(192, 168, 4, 1);
	IPAddress nMask(255, 255, 255, 0);
	WiFi.softAPConfig(ip, ip, nMask);

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("WiFi access point started. Listening on: ") + WiFi.softAPIP().toString());

	return true;
}

/**
 * @brief Connect to a WiFi network.
 * @param ssid ssid of the WiFi network
 * @param password password of the WiFi network
 * @param timeout connection timeout
 * @return true when successful
 * @return false when there was an error
 */
bool TL::WiFiManager::connectTo(const char *ssid, const char *password, const uint32_t timeout)
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Connecting to WiFi newtork '") + String(ssid) + F("' with password '") + String(password) + F("'. This can take a few seconds."));
	if (strlen(ssid) < 4 && strlen(password) < 8)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("WiFi SSID or Password too short and invalid."));
		return false;
	}

	WiFi.begin(ssid, password);
	WiFi.setAutoReconnect(true);

	uint64_t start = millis();
	while (WiFi.status() != WL_CONNECTED)
	{
		if (millis() - start > timeout)
		{
			TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Failed to connected to WiFi network. The connection timed out."));
			return false;
		}
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Successfully connected to WiFi network."));
	return true;
}
