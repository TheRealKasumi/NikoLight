/**
 * @file WiFiConfigurationEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to configure the WiFi settings.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "server/WiFiConfigurationEndpoint.h"

// Initialize
TL::Configuration *TL::WiFiConfigurationEndpoint::configuration = nullptr;
std::function<bool()> TL::WiFiConfigurationEndpoint::configChangedCallback = nullptr;

/**
 * @brief Add all request handler for this {@link TL::RestEndpoint} to the {@link TL::WebServerManager}.
 */
void TL::WiFiConfigurationEndpoint::begin(TL::Configuration *_configuration, std::function<bool()> _configChangedCallback)
{
	TL::WiFiConfigurationEndpoint::configuration = _configuration;
	TL::WiFiConfigurationEndpoint::configChangedCallback = _configChangedCallback;
	TL::WiFiConfigurationEndpoint::webServerManager->addRequestHandler((getBaseUri() + F("config/wifi")).c_str(), http_method::HTTP_GET, TL::WiFiConfigurationEndpoint::getWiFiConfig);
	TL::WiFiConfigurationEndpoint::webServerManager->addRequestHandler((getBaseUri() + F("config/wifi")).c_str(), http_method::HTTP_POST, TL::WiFiConfigurationEndpoint::postWiFiConfig);
}

/**
 * @brief Return the WiFi configuration to the client.
 */
void TL::WiFiConfigurationEndpoint::getWiFiConfig()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the WiFi configuration."));

	DynamicJsonDocument jsonDoc(1024);
	JsonObject config = jsonDoc.createNestedObject(F("wifiConfig"));
	config[F("accessPointSsid")] = TL::WiFiConfigurationEndpoint::configuration->getWiFiConfig().accessPointSsid;
	config[F("accessPointPassword")] = TL::WiFiConfigurationEndpoint::configuration->getWiFiConfig().accessPointPassword;
	config[F("accessPointChannel")] = TL::WiFiConfigurationEndpoint::configuration->getWiFiConfig().accessPointChannel;
	config[F("accessPointHidden")] = TL::WiFiConfigurationEndpoint::configuration->getWiFiConfig().accessPointHidden;
	config[F("accessPointMaxConnections")] = TL::WiFiConfigurationEndpoint::configuration->getWiFiConfig().accessPointMaxConnections;
	// config[F("wifiSsid")] = TL::WiFiConfigurationEndpoint::configuration->getWiFiConfig().wifiSsid;
	// config[F("wifiPassword")] = TL::WiFiConfigurationEndpoint::configuration->getWiFiConfig().wifiPassword;

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::WiFiConfigurationEndpoint::sendJsonDocument(200, F("Here is your WiFi configuration."), jsonDoc);
}

/**
 * @brief Receive the WiFi configuration sent by the client.
 */
void TL::WiFiConfigurationEndpoint::postWiFiConfig()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the WiFi configuration."));

	if (!TL::WiFiConfigurationEndpoint::webServer->hasHeader(F("content-type")) || TL::WiFiConfigurationEndpoint::webServer->header(F("content-type")) != F("application/json"))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/json\"."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The content type must be \"application/json\"."));
		return;
	}

	if (!TL::WiFiConfigurationEndpoint::webServer->hasArg(F("plain")))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a valid json body with the system configuration."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("There must be a valid json body with the system configuration."));
		return;
	}

	const String body = TL::WiFiConfigurationEndpoint::webServer->arg(F("plain"));
	if (body.length() == 0 || body.length() > 1024)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 1024 bytes."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 1024 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(1024);
	if (!TL::WiFiConfigurationEndpoint::parseJsonDocument(jsonDoc, body))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body could not be parsed. The json is invalid."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The body could not be parsed. The json is invalid."));
		return;
	}

	if (!jsonDoc[F("wifiConfig")].is<JsonObject>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The json must contain the \"wifiConfig\" object."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The json must contain the \"wifiConfig\" object."));
		return;
	}

	JsonObject configuration = jsonDoc[F("wifiConfig")].as<JsonObject>();
	if (!TL::WiFiConfigurationEndpoint::validateConfiguration(configuration))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The validation of the WiFi configuration failed."));
		return;
	}

	TL::Configuration::WiFiConfig config = TL::WiFiConfigurationEndpoint::configuration->getWiFiConfig();
	config.accessPointSsid = configuration[F("accessPointSsid")].as<String>();
	config.accessPointPassword = configuration[F("accessPointPassword")].as<String>();
	config.accessPointChannel = configuration[F("accessPointChannel")].as<uint8_t>();
	config.accessPointHidden = configuration[F("accessPointHidden")].as<bool>();
	config.accessPointMaxConnections = configuration[F("accessPointMaxConnections")].as<uint8_t>();
	// config.wifiSsid = configuration[F("wifiSsid")].as<String>();
	// config.wifiPassword = configuration[F("wifiPassword")].as<String>();

	TL::WiFiConfigurationEndpoint::configuration->setWiFiConfig(config);
	if (TL::WiFiConfigurationEndpoint::configuration->save())
	{
		if (TL::WiFiConfigurationEndpoint::configChangedCallback)
		{
			if (!TL::WiFiConfigurationEndpoint::configChangedCallback())
			{
				TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The callback function returned with an error."));
				TL::WiFiConfigurationEndpoint::sendSimpleResponse(500, F("Failed to apply WiFi configuration."));
				return;
			}
		}
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save WiFi configuration."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save WiFi configuration."));
		return;
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi configuration saved. Sending the response."));
	TL::WiFiConfigurationEndpoint::sendSimpleResponse(200, F("I update the WiFi configuration for you. Please don't forget to reconnect."));
}

/**
 * @brief Validate if a wifi configuration is valid.
 * @param jsonObject json object holding the configuration
 * @return true when valid
 * @return false when invalid
 */
bool TL::WiFiConfigurationEndpoint::validateConfiguration(const JsonObject &jsonObject)
{
	if (!jsonObject[F("accessPointSsid")].is<String>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"accessPointSsid\" field in configuration must be of type \"string\"."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"accessPointSsid\" field in configuration must be of type \"string\"."));
		return false;
	}

	if (!TL::WiFiConfigurationEndpoint::validateWiFiSsid(jsonObject[F("accessPointSsid")].as<String>()))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"accessPointSsid\" field is invalid. It must be a valid WiFi SSID."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"accessPointSsid\" field is invalid. It must be a valid WiFi SSID."));
		return false;
	}

	if (!jsonObject[F("accessPointPassword")].is<String>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"accessPointPassword\" field in configuration must be of type \"string\"."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"accessPointPassword\" field in configuration must be of type \"string\"."));
		return false;
	}

	if (!TL::WiFiConfigurationEndpoint::validateWiFiPassword(jsonObject[F("accessPointPassword")].as<String>()))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"accessPointPassword\" field is invalid. It must be a valid WiFi password."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"accessPointPassword\" field is invalid. It must be a valid WiFi password."));
		return false;
	}

	if (!jsonObject[F("accessPointChannel")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"accessPointChannel\" field in configuration must be of type \"uint8\"."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"accessPointChannel\" field in configuration must be of type \"uint8\"."));
		return false;
	}

	if (!TL::WiFiConfigurationEndpoint::isInRange(jsonObject[F("accessPointChannel")].as<uint8_t>(), 0, 12))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"accessPointChannel\" field is invalid. It must be a valid WiFi channel."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"accessPointChannel\" field is invalid. It must be a valid WiFi channel."));
		return false;
	}

	if (!jsonObject[F("accessPointHidden")].is<bool>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"accessPointHidden\" field in configuration must be of type \"bool\"."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"accessPointHidden\" field in configuration must be of type \"bool\"."));
		return false;
	}

	if (!jsonObject[F("accessPointMaxConnections")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"accessPointMaxConnections\" field in configuration must be of type \"uint8\"."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"accessPointMaxConnections\" field in configuration must be of type \"uint8\"."));
		return false;
	}

	if (!TL::WiFiConfigurationEndpoint::isInRange(jsonObject[F("accessPointMaxConnections")].as<uint8_t>(), 1, 4))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"accessPointMaxConnections\" field is invalid. It must be between 1 and 4."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"accessPointMaxConnections\" field is invalid. It must be between 1 and 4."));
		return false;
	}

	return true;
}

/**
 * @brief Check if a value is in range.
 * @param value value to check
 * @param min minimum value
 * @param max maximum value
 * @return true when the value is valid
 * @return false when the value is invalid
 */
bool TL::WiFiConfigurationEndpoint::isInRange(const long value, const long min, const long max)
{
	return value >= min && value <= max;
}

/**
 * @brief Verify if a WiFi ssid is valid.
 * @param ssid ssid to check
 * @return true when valid
 * @return false when invalid
 */
bool TL::WiFiConfigurationEndpoint::validateWiFiSsid(const String ssid)
{
	if (ssid.length() < 2 || ssid.length() > 32)
	{
		return false;
	}

	const String validChars = F("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-_ ");
	for (uint8_t i = 0; i < ssid.length(); i++)
	{
		if (validChars.indexOf(ssid.charAt(i)) == -1)
		{
			return false;
		}
	}

	return true;
}

/**
 * @brief Verify if a WiFi password is valid.
 * @param password password to check
 * @return true when valid
 * @return false when invalid
 */
bool TL::WiFiConfigurationEndpoint::validateWiFiPassword(const String password)
{
	if (password.length() < 8 || password.length() > 63)
	{
		return false;
	}

	const String validChars = F("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+-_!\"§$%&/()=?*# ");
	for (uint8_t i = 0; i < password.length(); i++)
	{
		if (validChars.indexOf(password.charAt(i)) == -1)
		{
			return false;
		}
	}

	return true;
}
