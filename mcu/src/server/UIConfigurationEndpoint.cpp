/**
 * @file UIConfigurationEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to read and save UI related settings.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "server/UIConfigurationEndpoint.h"

/**
 * @brief Add all request handler for this {@link TL::RestEndpoint} to the {@link TL::WebServerManager}.
 */
void TL::UIConfigurationEndpoint::begin()
{
	TL::WebServerManager::addRequestHandler((getBaseUri() + F("config/ui")).c_str(), http_method::HTTP_GET, TL::UIConfigurationEndpoint::getUIConfig);
	TL::WebServerManager::addRequestHandler((getBaseUri() + F("config/ui")).c_str(), http_method::HTTP_POST, TL::UIConfigurationEndpoint::postUIConfig);
}

/**
 * @brief Return the UI configuration to the client.
 */
void TL::UIConfigurationEndpoint::getUIConfig()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the UI configuration."));
	if (!TL::Configuration::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TesLight configuration was not initialized. Can not access configuration."));
		TL::UIConfigurationEndpoint::sendSimpleResponse(500, F("The TesLight configuration was not initialized. Can not access configuration."));
		return;
	}

	DynamicJsonDocument jsonDoc(256);
	const JsonObject uiConfig = jsonDoc.createNestedObject(F("uiConfig"));
	uiConfig[F("firmware")] = TL::Configuration::getUIConfiguration().firmware;
	uiConfig[F("language")] = TL::Configuration::getUIConfiguration().language;
	uiConfig[F("theme")] = TL::Configuration::getUIConfiguration().theme;
	uiConfig[F("expertMode")] = TL::Configuration::getUIConfiguration().expertMode;

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::UIConfigurationEndpoint::sendJsonDocument(200, F("Here is your UI configuration."), jsonDoc);
}

/**
 * @brief Receive the UI configuration sent by the client.
 */
void TL::UIConfigurationEndpoint::postUIConfig()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the UI configuration."));
	if (!TL::Configuration::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TesLight configuration was not initialized. Can not access configuration."));
		TL::UIConfigurationEndpoint::sendSimpleResponse(500, F("The TesLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!TL::UIConfigurationEndpoint::webServer->hasHeader(F("content-type")) || TL::UIConfigurationEndpoint::webServer->header(F("content-type")) != F("application/json"))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/json\"."));
		TL::UIConfigurationEndpoint::sendSimpleResponse(400, F("The content type must be \"application/json\"."));
		return;
	}

	if (!TL::UIConfigurationEndpoint::webServer->hasArg(F("plain")))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a valid json body with the UI configuration."));
		TL::UIConfigurationEndpoint::sendSimpleResponse(400, F("There must be a valid json body with the UI configuration."));
		return;
	}

	const String body = TL::UIConfigurationEndpoint::webServer->arg(F("plain"));
	if (body.length() == 0 || body.length() > 256)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 256 bytes."));
		TL::UIConfigurationEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 256 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(256);
	if (!TL::UIConfigurationEndpoint::parseJsonDocument(jsonDoc, body))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body could not be parsed. The json is invalid."));
		TL::UIConfigurationEndpoint::sendSimpleResponse(400, F("The body could not be parsed. The json is invalid."));
		return;
	}

	if (!jsonDoc[F("uiConfig")].is<JsonObject>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The json must contain a \"uiConfig\" object."));
		TL::UIConfigurationEndpoint::sendSimpleResponse(400, F("The json must contain a \"uiConfig\" object."));
		return;
	}

	const JsonObject uiConfig = jsonDoc[F("uiConfig")].as<JsonObject>();
	if (!TL::UIConfigurationEndpoint::validateUIConfig(uiConfig))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The validation of the configuration failed."));
		return;
	}

	TL::Configuration::UIConfiguration uiConfiguration;
	uiConfiguration.firmware = FW_VERSION;
	uiConfiguration.language = uiConfig[F("language")].as<String>();
	uiConfiguration.theme = uiConfig[F("theme")].as<String>();
	uiConfiguration.expertMode = uiConfig[F("expertMode")].as<bool>();

	TL::Configuration::setUIConfiguration(uiConfiguration);
	const TL::Configuration::Error configSaveError = TL::Configuration::save();
	if (configSaveError == TL::Configuration::Error::ERROR_FILE_OPEN)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save ui configuration. The configuration file could not be opened."));
		TL::UIConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save ui configuration. The configuration file could not be opened."));
		return;
	}
	else if (configSaveError == TL::Configuration::Error::ERROR_FILE_WRITE)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save ui configuration. The configuration file could not be written."));
		TL::UIConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save ui configuration. The configuration file could not be written."));
		return;
	}
	else if (configSaveError != TL::Configuration::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save ui configuration."));
		TL::UIConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save ui configuration."));
		return;
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("UI configuration saved. Sending the response."));
	TL::UIConfigurationEndpoint::sendSimpleResponse(200, F("Configuration saved! Thank you for telling me your personal preferences! Now I know you even better."));
}

/**
 * @brief Validate if the UI configuration is valid.
 * @param jsonObject json object holding the configuration for the UI
 * @return true when valid
 * @return false when invalid
 */
bool TL::UIConfigurationEndpoint::validateUIConfig(const JsonObject &jsonObject)
{
	if (!jsonObject[F("language")].is<String>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"language\" field must be of type \"string\"."));
		TL::UIConfigurationEndpoint::sendSimpleResponse(400, F("The \"language\" field must be of type \"string\"."));
		return false;
	}

	if (!jsonObject[F("theme")].is<String>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"theme\" field must be of type \"string\"."));
		TL::UIConfigurationEndpoint::sendSimpleResponse(400, F("The \"theme\" field must be of type \"string\"."));
		return false;
	}

	if (!jsonObject[F("expertMode")].is<bool>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"expertMode\" field must be of type \"boolean\"."));
		TL::UIConfigurationEndpoint::sendSimpleResponse(400, F("The \"expertMode\" field must be of type \"boolean\"."));
		return false;
	}

	return true;
}
