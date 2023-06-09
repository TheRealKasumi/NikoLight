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
 * @brief Add all request handler for this {@link NL::RestEndpoint} to the {@link NL::WebServerManager}.
 */
void NL::UIConfigurationEndpoint::begin()
{
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/ui")).c_str(), http_method::HTTP_GET, NL::UIConfigurationEndpoint::getUIConfig);
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/ui")).c_str(), http_method::HTTP_PATCH, NL::UIConfigurationEndpoint::patchUIConfig);
}

/**
 * @brief Return the UI configuration to the client.
 */
void NL::UIConfigurationEndpoint::getUIConfig()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the UI configuration."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::UIConfigurationEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	DynamicJsonDocument jsonDoc(256);
	const JsonObject uiConfig = jsonDoc.createNestedObject(F("uiConfig"));
	uiConfig[F("firmware")] = NL::Configuration::getUIConfiguration().firmware;
	uiConfig[F("language")] = NL::Configuration::getUIConfiguration().language;
	uiConfig[F("theme")] = NL::Configuration::getUIConfiguration().theme;
	uiConfig[F("expertMode")] = NL::Configuration::getUIConfiguration().expertMode;

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	NL::UIConfigurationEndpoint::sendJsonDocument(200, F("Here is your UI configuration."), jsonDoc);
}

/**
 * @brief Update the UI configuration.
 */
void NL::UIConfigurationEndpoint::patchUIConfig()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the UI configuration."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::UIConfigurationEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!NL::UIConfigurationEndpoint::webServer->hasHeader(F("content-type")) || NL::UIConfigurationEndpoint::webServer->header(F("content-type")) != F("application/json"))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/json\"."));
		NL::UIConfigurationEndpoint::sendSimpleResponse(400, F("The content type must be \"application/json\"."));
		return;
	}

	if (!NL::UIConfigurationEndpoint::webServer->hasArg(F("plain")))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a valid json body with the UI configuration."));
		NL::UIConfigurationEndpoint::sendSimpleResponse(400, F("There must be a valid json body with the UI configuration."));
		return;
	}

	const String body = NL::UIConfigurationEndpoint::webServer->arg(F("plain"));
	if (body.length() == 0 || body.length() > 256)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 256 bytes."));
		NL::UIConfigurationEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 256 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(256);
	if (!NL::UIConfigurationEndpoint::parseJsonDocument(jsonDoc, body))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body could not be parsed. The json is invalid."));
		NL::UIConfigurationEndpoint::sendSimpleResponse(400, F("The body could not be parsed. The json is invalid."));
		return;
	}

	if (!jsonDoc[F("uiConfig")].is<JsonObject>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The json must contain a \"uiConfig\" object."));
		NL::UIConfigurationEndpoint::sendSimpleResponse(400, F("The json must contain a \"uiConfig\" object."));
		return;
	}

	const JsonObject uiConfig = jsonDoc[F("uiConfig")].as<JsonObject>();
	if (!NL::UIConfigurationEndpoint::validateUIConfig(uiConfig))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The validation of the configuration failed."));
		return;
	}

	NL::Configuration::UIConfiguration uiConfiguration;
	uiConfiguration.firmware = FW_VERSION;
	uiConfiguration.language = uiConfig[F("language")].as<String>();
	uiConfiguration.theme = uiConfig[F("theme")].as<String>();
	uiConfiguration.expertMode = uiConfig[F("expertMode")].as<bool>();

	NL::Configuration::setUIConfiguration(uiConfiguration);
	const NL::Configuration::Error configSaveError = NL::Configuration::save();
	if (configSaveError == NL::Configuration::Error::ERROR_FILE_OPEN)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save ui configuration. The configuration file could not be opened."));
		NL::UIConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save ui configuration. The configuration file could not be opened."));
		return;
	}
	else if (configSaveError == NL::Configuration::Error::ERROR_FILE_WRITE)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save ui configuration. The configuration file could not be written."));
		NL::UIConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save ui configuration. The configuration file could not be written."));
		return;
	}
	else if (configSaveError != NL::Configuration::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save ui configuration."));
		NL::UIConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save ui configuration."));
		return;
	}

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("UI configuration saved. Sending the response."));
	NL::UIConfigurationEndpoint::sendSimpleResponse(200, F("Configuration saved! Thank you for telling me your personal preferences! Now I know you even better."));
}

/**
 * @brief Validate if the UI configuration is valid.
 * @param jsonObject json object holding the configuration for the UI
 * @return true when valid
 * @return false when invalid
 */
bool NL::UIConfigurationEndpoint::validateUIConfig(const JsonObject &jsonObject)
{
	if (!jsonObject[F("language")].is<String>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"language\" field must be of type \"string\"."));
		NL::UIConfigurationEndpoint::sendSimpleResponse(400, F("The \"language\" field must be of type \"string\"."));
		return false;
	}

	if (!jsonObject[F("theme")].is<String>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"theme\" field must be of type \"string\"."));
		NL::UIConfigurationEndpoint::sendSimpleResponse(400, F("The \"theme\" field must be of type \"string\"."));
		return false;
	}

	if (!jsonObject[F("expertMode")].is<bool>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"expertMode\" field must be of type \"boolean\"."));
		NL::UIConfigurationEndpoint::sendSimpleResponse(400, F("The \"expertMode\" field must be of type \"boolean\"."));
		return false;
	}

	return true;
}
