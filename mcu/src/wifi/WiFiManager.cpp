/**
 * @file WiFiManager.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::WiFiManager}.
 * @version 0.0.1
 * @date 2022-06-28
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "wifi/WiFiManager.h"

/**
 * @brief Create a new instance of {@link TesLight::WiFiManager}.
 */
TesLight::WiFiManager::WiFiManager()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("WebServer.cpp:WebServer"), F("Initializing WiFi manager."));
	WiFi.mode(WIFI_MODE_APSTA);
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("WebServer.cpp:WebServer"), F("WiFi manager initialized."));
}

/**
 * @brief Destroy the {@link TesLight::WiFiManager} instance.
 */
TesLight::WiFiManager::~WiFiManager()
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
bool TesLight::WiFiManager::startAccessPoint(const char *ssid, const char *password, uint8_t channel, bool hidden, uint8_t maxConnections)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("WebServer.cpp:startAccessPoint"), (String)F("Starting WiFi access point with ssid '") + String(ssid) + F("' and password '") + String(password) + F("' on channel ") + channel + F("."));

	if (hidden)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("WebServer.cpp:startAccessPoint"), F("WiFi access point is configured to start in hidden mode."));
	}

	if (!WiFi.softAP(ssid, strlen(password) > 0 ? password : nullptr, channel, hidden, maxConnections))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("WebServer.cpp:startAccessPoint"), F("WiFi access point could not be started."));
		return false;
	}

	const IPAddress apIP = WiFi.softAPIP();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("WebServer.cpp:startAccessPoint"), (String)F("WiFi access point started. Listening on: ") + apIP.toString());

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
bool TesLight::WiFiManager::connectTo(const char *ssid, const char *password, const uint32_t timeout)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("WebServer.cpp:startAccessPoint"), (String)F("Connecting to WiFi newtork '") + String(ssid) + F("' with password '") + String(password) + F("'. This can take a few seconds."));
	WiFi.begin(ssid, password);

	uint64_t start = millis();
	while (WiFi.status() != WL_CONNECTED)
	{
		if (millis() - start > timeout)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("WebServer.cpp:startAccessPoint"), F("Failed to connected to WiFi network. The connection timed out."));
			return false;
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("WebServer.cpp:startAccessPoint"), F("Successfully connected to WiFi network."));
	return true;
}
