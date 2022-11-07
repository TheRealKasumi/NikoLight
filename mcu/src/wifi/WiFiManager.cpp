/**
 * @file WiFiManager.cpp
 * @author TheRealKasumi
 * @brief Implementation of the {@link TesLight::WiFiManager}.
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
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initializing WiFi manager."));
	WiFi.mode(WIFI_MODE_APSTA);
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi manager initialized."));
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
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Starting WiFi access point with ssid '") + String(ssid) + F("' and password '") + String(password) + F("' on channel ") + channel + F("."));

	if (hidden)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi access point is configured to start in hidden mode."));
	}

	if (!WiFi.softAP(ssid, strlen(password) > 0 ? password : nullptr, channel, hidden, maxConnections))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi access point could not be started."));
		return false;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Starting WiFi access point."));
	delay(100);

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Configuring WiFi access point."));
	IPAddress ip(192, 168, 4, 1);
	IPAddress nMask(255, 255, 255, 0);
	WiFi.softAPConfig(ip, ip, nMask);

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("WiFi access point started. Listening on: ") + WiFi.softAPIP().toString());

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
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Connecting to WiFi newtork '") + String(ssid) + F("' with password '") + String(password) + F("'. This can take a few seconds."));
	if (strlen(ssid) < 4 && strlen(password) < 8)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("WiFi SSID or Password too short and invalid."));
		return false;
	}

	WiFi.begin(ssid, password);
	WiFi.setAutoReconnect(true);

	uint64_t start = millis();
	while (WiFi.status() != WL_CONNECTED)
	{
		if (millis() - start > timeout)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Failed to connected to WiFi network. The connection timed out."));
			return false;
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Successfully connected to WiFi network."));
	return true;
}
