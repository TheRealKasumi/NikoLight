/**
 * @file WiFiConfigurationEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to configure the WiFi settings.
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
#include "server/WiFiConfigurationEndpoint.h"

/**
 * @brief Add all request handler for this {@link NL::RestEndpoint} to the {@link NL::WebServerManager}.
 */
void NL::WiFiConfigurationEndpoint::begin()
{
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/wifi")).c_str(), http_method::HTTP_GET, NL::WiFiConfigurationEndpoint::getWiFiConfig);
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/wifi")).c_str(), http_method::HTTP_PATCH, NL::WiFiConfigurationEndpoint::patchWiFiConfig);
}

/**
 * @brief Return the WiFi configuration to the client.
 */
void NL::WiFiConfigurationEndpoint::getWiFiConfig()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the WiFi configuration."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	DynamicJsonDocument jsonDoc(4096);
	const JsonObject config = jsonDoc.createNestedObject(F("wifiConfig"));
	config[F("accessPointSsid")] = NL::Configuration::getWiFiConfig().accessPointSsid;
	config[F("accessPointPassword")] = NL::Configuration::getWiFiConfig().accessPointPassword;
	config[F("accessPointChannel")] = NL::Configuration::getWiFiConfig().accessPointChannel;
	config[F("accessPointHidden")] = NL::Configuration::getWiFiConfig().accessPointHidden;
	config[F("accessPointMaxConnections")] = NL::Configuration::getWiFiConfig().accessPointMaxConnections;
	// config[F("wifiSsid")] = NL::Configuration::getWiFiConfig().wifiSsid;
	// config[F("wifiPassword")] = NL::Configuration::getWiFiConfig().wifiPassword;

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	NL::WiFiConfigurationEndpoint::sendJsonDocument(200, F("Here is your WiFi configuration."), jsonDoc);
}

/**
 * @brief Update the WiFi configuration.
 */
void NL::WiFiConfigurationEndpoint::patchWiFiConfig()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the WiFi configuration."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!NL::WiFiManager::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The WiFi manager was not initialized. Can not apply WiFi settings."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(500, F("The WiFi manager was not initialized. Can not apply WiFi settings."));
		return;
	}

	if (!NL::WiFiConfigurationEndpoint::webServer->hasHeader(F("content-type")) || NL::WiFiConfigurationEndpoint::webServer->header(F("content-type")) != F("application/json"))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/json\"."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The content type must be \"application/json\"."));
		return;
	}

	if (!NL::WiFiConfigurationEndpoint::webServer->hasArg(F("plain")))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a valid json body with the system configuration."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("There must be a valid json body with the system configuration."));
		return;
	}

	const String body = NL::WiFiConfigurationEndpoint::webServer->arg(F("plain"));
	if (body.length() == 0 || body.length() > 4096)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 4096 bytes."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 4096 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(4096);
	if (!NL::WiFiConfigurationEndpoint::parseJsonDocument(jsonDoc, body))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body could not be parsed. The json is invalid."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The body could not be parsed. The json is invalid."));
		return;
	}

	if (!jsonDoc[F("wifiConfig")].is<JsonObject>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The json must contain a \"wifiConfig\" object."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The json must contain a \"wifiConfig\" object."));
		return;
	}

	const JsonObject configuration = jsonDoc[F("wifiConfig")].as<JsonObject>();
	if (!NL::WiFiConfigurationEndpoint::validateConfiguration(configuration))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The validation of the WiFi configuration failed."));
		return;
	}

	NL::Configuration::WiFiConfig config;
	config.accessPointSsid = configuration[F("accessPointSsid")].as<String>();
	config.accessPointPassword = configuration[F("accessPointPassword")].as<String>();
	config.accessPointChannel = configuration[F("accessPointChannel")].as<uint8_t>();
	config.accessPointHidden = configuration[F("accessPointHidden")].as<bool>();
	config.accessPointMaxConnections = configuration[F("accessPointMaxConnections")].as<uint8_t>();
	// config.wifiSsid = configuration[F("wifiSsid")].as<String>();
	// config.wifiPassword = configuration[F("wifiPassword")].as<String>();

	if (NL::WiFiConfigurationEndpoint::hasChanged(config))
	{
		NL::Configuration::setWiFiConfig(config);
		const NL::Configuration::Error configSaveError = NL::Configuration::save();
		if (configSaveError == NL::Configuration::Error::ERROR_FILE_OPEN)
		{
			NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save WiFi configuration. The configuration file could not be opened."));
			NL::WiFiConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save WiFi configuration. The configuration file could not be opened."));
			return;
		}
		else if (configSaveError == NL::Configuration::Error::ERROR_FILE_WRITE)
		{
			NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save WiFi configuration. The configuration file could not be written."));
			NL::WiFiConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save WiFi configuration. The configuration file could not be written."));
			return;
		}
		else if (configSaveError != NL::Configuration::Error::OK)
		{
			NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save WiFi configuration."));
			NL::WiFiConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save WiFi configuration."));
			return;
		}

		const NL::WiFiManager::Error wifiError = NL::WiFiManager::startAccessPoint(NL::Configuration::getWiFiConfig().accessPointSsid.c_str(), NL::Configuration::getWiFiConfig().accessPointPassword.c_str(), NL::Configuration::getWiFiConfig().accessPointChannel, false, NL::Configuration::getWiFiConfig().accessPointMaxConnections);
		if (wifiError == NL::WiFiManager::Error::ERROR_START_AP)
		{
			NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point."));
			NL::WiFiConfigurationEndpoint::sendSimpleResponse(500, F("Failed to start WiFi access point."));
			return;
		}
		else if (wifiError == NL::WiFiManager::Error::ERROR_INVALID_SSID)
		{
			NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point. The SSID is invalid."));
			NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("Failed to start WiFi access point. The SSID is invalid."));
			return;
		}
		else if (wifiError == NL::WiFiManager::Error::ERROR_INVALID_PW)
		{
			NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point. The password is invalid."));
			NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("Failed to start WiFi access point. The password is invalid."));
			return;
		}
		else if (wifiError == NL::WiFiManager::Error::OK)
		{
			NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point."));
			NL::WiFiConfigurationEndpoint::sendSimpleResponse(500, F("Failed to start WiFi access point."));
			return;
		}
	}

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi configuration saved. Sending the response."));
	NL::WiFiConfigurationEndpoint::sendSimpleResponse(200, F("I update the WiFi configuration for you. Please don't forget to reconnect."));
}

/**
 * @brief Validate if a wifi configuration is valid.
 * @param jsonObject json object holding the configuration
 * @return true when valid
 * @return false when invalid
 */
bool NL::WiFiConfigurationEndpoint::validateConfiguration(const JsonObject &jsonObject)
{
	if (!jsonObject[F("accessPointSsid")].is<String>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accessPointSsid\" field must be of type \"string\"."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"accessPointSsid\" field must be of type \"string\"."));
		return false;
	}

	if (!NL::WiFiConfigurationEndpoint::validateWiFiSsid(jsonObject[F("accessPointSsid")].as<String>()))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accessPointSsid\" field is invalid. It must be a valid WiFi SSID."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"accessPointSsid\" field is invalid. It must be a valid WiFi SSID."));
		return false;
	}

	if (!jsonObject[F("accessPointPassword")].is<String>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accessPointPassword\" field must be of type \"string\"."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"accessPointPassword\" field must be of type \"string\"."));
		return false;
	}

	if (!NL::WiFiConfigurationEndpoint::validateWiFiPassword(jsonObject[F("accessPointPassword")].as<String>()))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accessPointPassword\" field is invalid. It must be a valid WiFi password."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"accessPointPassword\" field is invalid. It must be a valid WiFi password."));
		return false;
	}

	if (!jsonObject[F("accessPointChannel")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accessPointChannel\" field must be of type \"uint8\"."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"accessPointChannel\" field must be of type \"uint8\"."));
		return false;
	}

	if (!NL::WiFiConfigurationEndpoint::isInRange(jsonObject[F("accessPointChannel")].as<uint8_t>(), 0, 12))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accessPointChannel\" field is invalid. It must be a valid WiFi channel."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"accessPointChannel\" field is invalid. It must be a valid WiFi channel."));
		return false;
	}

	if (!jsonObject[F("accessPointHidden")].is<bool>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accessPointHidden\" field must be of type \"bool\"."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"accessPointHidden\" field must be of type \"bool\"."));
		return false;
	}

	if (!jsonObject[F("accessPointMaxConnections")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accessPointMaxConnections\" field must be of type \"uint8\"."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"accessPointMaxConnections\" field must be of type \"uint8\"."));
		return false;
	}

	if (!NL::WiFiConfigurationEndpoint::isInRange(jsonObject[F("accessPointMaxConnections")].as<uint8_t>(), 1, 4))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accessPointMaxConnections\" field is invalid. It must be between 1 and 4."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"accessPointMaxConnections\" field is invalid. It must be between 1 and 4."));
		return false;
	}

	/*if (!jsonObject[F("wifiSsid")].is<String>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"wifiSsid\" field must be of type \"string\"."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"wifiSsid\" field must be of type \"string\"."));
		return false;
	}

	if (!NL::WiFiConfigurationEndpoint::validateWiFiSsid(jsonObject[F("wifiSsid")].as<String>()))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"wifiSsid\" field is invalid. It must be a valid WiFi SSID."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"wifiSsid\" field is invalid. It must be a valid WiFi SSID."));
		return false;
	}

	if (!jsonObject[F("wifiPassword")].is<String>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"wifiPassword\" field must be of type \"string\"."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"wifiPassword\" field must be of type \"string\"."));
		return false;
	}

	if (!NL::WiFiConfigurationEndpoint::validateWiFiPassword(jsonObject[F("wifiPassword")].as<String>()))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"wifiPassword\" field is invalid. It must be a valid WiFi password."));
		NL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"wifiPassword\" field is invalid. It must be a valid WiFi password."));
		return false;
	}*/

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
bool NL::WiFiConfigurationEndpoint::isInRange(const long value, const long min, const long max)
{
	return value >= min && value <= max;
}

/**
 * @brief Verify if a WiFi ssid is valid.
 * @param ssid ssid to check
 * @return true when valid
 * @return false when invalid
 */
bool NL::WiFiConfigurationEndpoint::validateWiFiSsid(const String ssid)
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
bool NL::WiFiConfigurationEndpoint::validateWiFiPassword(const String password)
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

/**
 * @brief Check if the configuration has changed.
 * @param newConfig updated configuration
 * @return true when the configuration has changed
 * @return false when the configurationn has not changed
 */
bool NL::WiFiConfigurationEndpoint::hasChanged(const NL::Configuration::WiFiConfig &newConfig)
{
	const NL::Configuration::WiFiConfig oldConfig = NL::Configuration::getWiFiConfig();
	if (newConfig.accessPointSsid != oldConfig.accessPointSsid)
	{
		return true;
	}
	if (newConfig.accessPointPassword != oldConfig.accessPointPassword)
	{
		return true;
	}
	if (newConfig.accessPointChannel != oldConfig.accessPointChannel)
	{
		return true;
	}
	if (newConfig.accessPointHidden != oldConfig.accessPointHidden)
	{
		return true;
	}
	if (newConfig.accessPointMaxConnections != oldConfig.accessPointMaxConnections)
	{
		return true;
	}
	if (newConfig.wifiSsid != oldConfig.wifiSsid)
	{
		return true;
	}
	if (newConfig.wifiPassword != oldConfig.wifiPassword)
	{
		return true;
	}
	return false;
}
