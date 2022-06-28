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
 * @param logger instance of {@link TesLight::Logger}
 */
TesLight::WiFiManager::WiFiManager(TesLight::Logger *logger)
{
	this->logger = logger;
	init();
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
	this->logger->info(F("WiFiManager.cpp:startAccessPoint"), (String)F("Starting WiFi access point with ssid '") + String(ssid) + F("' and password '") + String(password) + F("' on channel ") + channel + F("."));

	if (hidden)
	{
		this->logger->warn(F("WiFiManager.cpp:startAccessPoint"), F("WiFi access point is configured to start in hidden mode."));
	}

	if (!WiFi.softAP(ssid, strlen(password) > 0 ? password : nullptr, channel, hidden, maxConnections))
	{
		this->logger->warn(F("WiFiManager.cpp:startAccessPoint"), F("WiFi access point could not be started."));
		return false;
	}

	const IPAddress apIP = WiFi.softAPIP();
	this->logger->info(F("WiFiManager.cpp:startAccessPoint"), (String)F("WiFi access point started. Listening on: ") + apIP.toString());

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
	this->logger->info(F("WiFiManager.cpp:connectTo"), (String)F("Connecting to WiFi newtork '") + String(ssid) + F("' with password '") + String(password) + F("'. This can take a few seconds."));
	WiFi.begin(ssid, password);

	uint64_t start = millis();
	while (WiFi.status() != WL_CONNECTED)
	{
		if (millis() - start > timeout)
		{
			this->logger->error(F("WiFiManager.cpp:connectTo"), F("Failed to connected to WiFi network. The connection timed out."));
			return false;
		}
	}

	this->logger->info(F("WiFiManager.cpp:connectTo"), F("Successfully connected to WiFi network."));
	return true;
}

/**
 * @brief Initialize the {@link TesLight::WiFiManager}.
 */
void TesLight::WiFiManager::init()
{
	this->logger->info(F("WiFiManager.cpp:init"), F("Initializing WiFi."));
	WiFi.mode(WIFI_MODE_APSTA);
	this->logger->info(F("WiFiManager.cpp:init"), F("WiFi initialized, ready to connect to WiFi network or create AP."));
}
