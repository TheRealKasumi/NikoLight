/**
 * @file WiFiConfigurationEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to configure the WiFi settings.
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
#include "server/WiFiConfigurationEndpoint.h"

/**
 * @brief Add all request handler for this {@link TL::RestEndpoint} to the {@link TL::WebServerManager}.
 */
void TL::WiFiConfigurationEndpoint::begin()
{
	TL::WebServerManager::addRequestHandler((getBaseUri() + F("config/wifi")).c_str(), http_method::HTTP_GET, TL::WiFiConfigurationEndpoint::getWiFiConfig);
	TL::WebServerManager::addRequestHandler((getBaseUri() + F("config/wifi")).c_str(), http_method::HTTP_POST, TL::WiFiConfigurationEndpoint::postWiFiConfig);
}

/**
 * @brief Return the WiFi configuration to the client.
 */
void TL::WiFiConfigurationEndpoint::getWiFiConfig()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the WiFi configuration."));
	if (!TL::Configuration::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TesLight configuration was not initialized. Can not access configuration."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(500, F("The TesLight configuration was not initialized. Can not access configuration."));
		return;
	}

	DynamicJsonDocument jsonDoc(4096);
	const JsonObject config = jsonDoc.createNestedObject(F("wifiConfig"));
	config[F("accessPointSsid")] = TL::Configuration::getWiFiConfig().accessPointSsid;
	config[F("accessPointPassword")] = TL::Configuration::getWiFiConfig().accessPointPassword;
	config[F("accessPointChannel")] = TL::Configuration::getWiFiConfig().accessPointChannel;
	config[F("accessPointHidden")] = TL::Configuration::getWiFiConfig().accessPointHidden;
	config[F("accessPointMaxConnections")] = TL::Configuration::getWiFiConfig().accessPointMaxConnections;
	// config[F("wifiSsid")] = TL::Configuration::getWiFiConfig().wifiSsid;
	// config[F("wifiPassword")] = TL::Configuration::getWiFiConfig().wifiPassword;

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::WiFiConfigurationEndpoint::sendJsonDocument(200, F("Here is your WiFi configuration."), jsonDoc);
}

/**
 * @brief Receive the WiFi configuration sent by the client.
 */
void TL::WiFiConfigurationEndpoint::postWiFiConfig()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the WiFi configuration."));
	if (!TL::Configuration::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TesLight configuration was not initialized. Can not access configuration."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(500, F("The TesLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!TL::WiFiManager::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The WiFi manager was not initialized. Can not apply WiFi settings."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(500, F("The WiFi manager was not initialized. Can not apply WiFi settings."));
		return;
	}

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
	if (body.length() == 0 || body.length() > 4096)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 4096 bytes."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 4096 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(4096);
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

	const JsonObject configuration = jsonDoc[F("wifiConfig")].as<JsonObject>();
	if (!TL::WiFiConfigurationEndpoint::validateConfiguration(configuration))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The validation of the WiFi configuration failed."));
		return;
	}

	TL::Configuration::WiFiConfig config;
	config.accessPointSsid = configuration[F("accessPointSsid")].as<String>();
	config.accessPointPassword = configuration[F("accessPointPassword")].as<String>();
	config.accessPointChannel = configuration[F("accessPointChannel")].as<uint8_t>();
	config.accessPointHidden = configuration[F("accessPointHidden")].as<bool>();
	config.accessPointMaxConnections = configuration[F("accessPointMaxConnections")].as<uint8_t>();
	// config.wifiSsid = configuration[F("wifiSsid")].as<String>();
	// config.wifiPassword = configuration[F("wifiPassword")].as<String>();

	if (TL::WiFiConfigurationEndpoint::hasChanged(config))
	{
		TL::Configuration::setWiFiConfig(config);
		const TL::Configuration::Error configSaveError = TL::Configuration::save();
		if (configSaveError == TL::Configuration::Error::ERROR_FILE_OPEN)
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save WiFi configuration. The configuration file could not be opened."));
			TL::WiFiConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save WiFi configuration. The configuration file could not be opened."));
			return;
		}
		else if (configSaveError == TL::Configuration::Error::ERROR_FILE_WRITE)
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save WiFi configuration. The configuration file could not be written."));
			TL::WiFiConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save WiFi configuration. The configuration file could not be written."));
			return;
		}
		else if (configSaveError != TL::Configuration::Error::OK)
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save WiFi configuration."));
			TL::WiFiConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save WiFi configuration."));
			return;
		}

		const TL::WiFiManager::Error wifiError = TL::WiFiManager::startAccessPoint(TL::Configuration::getWiFiConfig().accessPointSsid.c_str(), TL::Configuration::getWiFiConfig().accessPointPassword.c_str(), TL::Configuration::getWiFiConfig().accessPointChannel, false, TL::Configuration::getWiFiConfig().accessPointMaxConnections);
		if (wifiError == TL::WiFiManager::Error::ERROR_START_AP)
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point."));
			TL::WiFiConfigurationEndpoint::sendSimpleResponse(500, F("Failed to start WiFi access point."));
			return;
		}
		else if (wifiError == TL::WiFiManager::Error::ERROR_INVALID_SSID)
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point. The SSID is invalid."));
			TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("Failed to start WiFi access point. The SSID is invalid."));
			return;
		}
		else if (wifiError == TL::WiFiManager::Error::ERROR_INVALID_PW)
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point. The password is invalid."));
			TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("Failed to start WiFi access point. The password is invalid."));
			return;
		}
		else
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point."));
			TL::WiFiConfigurationEndpoint::sendSimpleResponse(500, F("Failed to start WiFi access point."));
			return;
		}
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
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accessPointSsid\" field must be of type \"string\"."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"accessPointSsid\" field must be of type \"string\"."));
		return false;
	}

	if (!TL::WiFiConfigurationEndpoint::validateWiFiSsid(jsonObject[F("accessPointSsid")].as<String>()))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accessPointSsid\" field is invalid. It must be a valid WiFi SSID."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"accessPointSsid\" field is invalid. It must be a valid WiFi SSID."));
		return false;
	}

	if (!jsonObject[F("accessPointPassword")].is<String>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accessPointPassword\" field must be of type \"string\"."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"accessPointPassword\" field must be of type \"string\"."));
		return false;
	}

	if (!TL::WiFiConfigurationEndpoint::validateWiFiPassword(jsonObject[F("accessPointPassword")].as<String>()))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accessPointPassword\" field is invalid. It must be a valid WiFi password."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"accessPointPassword\" field is invalid. It must be a valid WiFi password."));
		return false;
	}

	if (!jsonObject[F("accessPointChannel")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accessPointChannel\" field must be of type \"uint8\"."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"accessPointChannel\" field must be of type \"uint8\"."));
		return false;
	}

	if (!TL::WiFiConfigurationEndpoint::isInRange(jsonObject[F("accessPointChannel")].as<uint8_t>(), 0, 12))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accessPointChannel\" field is invalid. It must be a valid WiFi channel."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"accessPointChannel\" field is invalid. It must be a valid WiFi channel."));
		return false;
	}

	if (!jsonObject[F("accessPointHidden")].is<bool>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accessPointHidden\" field must be of type \"bool\"."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"accessPointHidden\" field must be of type \"bool\"."));
		return false;
	}

	if (!jsonObject[F("accessPointMaxConnections")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accessPointMaxConnections\" field must be of type \"uint8\"."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"accessPointMaxConnections\" field must be of type \"uint8\"."));
		return false;
	}

	if (!TL::WiFiConfigurationEndpoint::isInRange(jsonObject[F("accessPointMaxConnections")].as<uint8_t>(), 1, 4))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"accessPointMaxConnections\" field is invalid. It must be between 1 and 4."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"accessPointMaxConnections\" field is invalid. It must be between 1 and 4."));
		return false;
	}

	/*if (!jsonObject[F("wifiSsid")].is<String>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"wifiSsid\" field must be of type \"string\"."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"wifiSsid\" field must be of type \"string\"."));
		return false;
	}

	if (!TL::WiFiConfigurationEndpoint::validateWiFiSsid(jsonObject[F("wifiSsid")].as<String>()))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"wifiSsid\" field is invalid. It must be a valid WiFi SSID."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"wifiSsid\" field is invalid. It must be a valid WiFi SSID."));
		return false;
	}

	if (!jsonObject[F("wifiPassword")].is<String>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"wifiPassword\" field must be of type \"string\"."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"wifiPassword\" field must be of type \"string\"."));
		return false;
	}

	if (!TL::WiFiConfigurationEndpoint::validateWiFiPassword(jsonObject[F("wifiPassword")].as<String>()))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"wifiPassword\" field is invalid. It must be a valid WiFi password."));
		TL::WiFiConfigurationEndpoint::sendSimpleResponse(400, F("The \"wifiPassword\" field is invalid. It must be a valid WiFi password."));
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

/**
 * @brief Check if the configuration has changed.
 * @param newConfig updated configuration
 * @return true when the configuration has changed
 * @return false when the configurationn has not changed
 */
bool TL::WiFiConfigurationEndpoint::hasChanged(const TL::Configuration::WiFiConfig &newConfig)
{
	const TL::Configuration::WiFiConfig oldConfig = TL::Configuration::getWiFiConfig();
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
