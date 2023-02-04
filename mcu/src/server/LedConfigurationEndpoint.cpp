/**
 * @file LedConfigurationEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to configure the LED settings.
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
#include "server/LedConfigurationEndpoint.h"

/**
 * @brief Add all request handler for this {@link TL::RestEndpoint} to the {@link TL::WebServerManager}.
 */
void TL::LedConfigurationEndpoint::begin()
{
	TL::WebServerManager::addRequestHandler((getBaseUri() + F("config/led")).c_str(), http_method::HTTP_GET, TL::LedConfigurationEndpoint::getLedConfig);
	TL::WebServerManager::addRequestHandler((getBaseUri() + F("config/led")).c_str(), http_method::HTTP_POST, TL::LedConfigurationEndpoint::postLedConfig);
}

/**
 * @brief Return the LED configuration to the client.
 */
void TL::LedConfigurationEndpoint::getLedConfig()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the LED configuration."));
	if (!TL::Configuration::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TesLight configuration was not initialized. Can not access configuration."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(500, F("The TesLight configuration was not initialized. Can not access configuration."));
		return;
	}

	DynamicJsonDocument jsonDoc(5600);
	const JsonArray ledConfig = jsonDoc.createNestedArray(F("ledConfig"));
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		const JsonObject ledZone = ledConfig.createNestedObject();
		ledZone[F("ledPin")] = TL::Configuration::getLedConfig(i).ledPin;
		ledZone[F("ledCount")] = TL::Configuration::getLedConfig(i).ledCount;
		ledZone[F("type")] = TL::Configuration::getLedConfig(i).type;
		ledZone[F("dataSource")] = TL::Configuration::getLedConfig(i).dataSource;
		ledZone[F("speed")] = TL::Configuration::getLedConfig(i).speed;
		ledZone[F("offset")] = TL::Configuration::getLedConfig(i).offset;
		ledZone[F("brightness")] = TL::Configuration::getLedConfig(i).brightness;
		ledZone[F("reverse")] = TL::Configuration::getLedConfig(i).reverse;
		ledZone[F("fadeSpeed")] = TL::Configuration::getLedConfig(i).fadeSpeed;
		ledZone[F("ledVoltage")] = TL::Configuration::getLedConfig(i).ledVoltage;

		const JsonArray animationSettings = ledZone.createNestedArray(F("animationSettings"));
		for (uint8_t j = 0; j < ANIMATOR_NUM_ANIMATION_SETTINGS; j++)
		{
			animationSettings.add(TL::Configuration::getLedConfig(i).animationSettings[j]);
		}

		const JsonArray channelCurrents = ledZone.createNestedArray(F("channelCurrents"));
		for (uint8_t j = 0; j < 3; j++)
		{
			channelCurrents.add(TL::Configuration::getLedConfig(i).ledChannelCurrent[j]);
		}
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::LedConfigurationEndpoint::sendJsonDocument(200, F("Here is your LED configuration. Looks shiny :3 ."), jsonDoc);
}

/**
 * @brief Receive the LED configuration sent by the client.
 */
void TL::LedConfigurationEndpoint::postLedConfig()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the LED configuration."));
	if (!TL::Configuration::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TesLight configuration was not initialized. Can not access configuration."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(500, F("The TesLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!TL::LedConfigurationEndpoint::webServer->hasHeader(F("content-type")) || TL::LedConfigurationEndpoint::webServer->header(F("content-type")) != F("application/json"))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/json\"."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, F("The content type must be \"application/json\"."));
		return;
	}

	if (!TL::LedConfigurationEndpoint::webServer->hasArg(F("plain")))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a valid json body with the LED configuration."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, F("There must be a valid json body with the LED configuration."));
		return;
	}

	const String body = TL::LedConfigurationEndpoint::webServer->arg(F("plain"));
	if (body.length() == 0 || body.length() > 4096)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 4096 bytes."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 4096 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(5600);
	if (!TL::LedConfigurationEndpoint::parseJsonDocument(jsonDoc, body))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body could not be parsed. The json is invalid."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, F("The body could not be parsed. The json is invalid."));
		return;
	}

	if (!jsonDoc[F("ledConfig")].is<JsonArray>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The json must contain the \"ledConfig\" array."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, F("The json must contain the \"ledConfig\" array."));
		return;
	}

	const JsonArray ledConfig = jsonDoc[F("ledConfig")].as<JsonArray>();
	if (ledConfig.size() != LED_NUM_ZONES)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"ledConfig\" must contain exactly ") + LED_NUM_ZONES + F(" zones."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"ledConfig\" must contain exactly ") + LED_NUM_ZONES + F(" zones."));
		return;
	}

	TL::Configuration::LedConfig config[LED_NUM_ZONES];
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		const JsonObject ledZone = ledConfig[i];
		if (!TL::LedConfigurationEndpoint::validateLedZone(ledZone, i))
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The validation of the LED configuration failed."));
			return;
		}

		config[i].ledPin = ledZone[F("ledPin")].as<uint8_t>();
		config[i].ledCount = ledZone[F("ledCount")].as<uint16_t>();
		config[i].type = ledZone[F("type")].as<uint8_t>();
		config[i].dataSource = ledZone[F("dataSource")].as<uint8_t>();
		config[i].speed = ledZone[F("speed")].as<uint8_t>();
		config[i].offset = ledZone[F("offset")].as<uint16_t>();
		config[i].brightness = ledZone[F("brightness")].as<uint8_t>();
		config[i].reverse = ledZone[F("reverse")].as<bool>();
		config[i].fadeSpeed = ledZone[F("fadeSpeed")].as<uint8_t>();
		config[i].ledVoltage = ledZone[F("ledVoltage")].as<float>();

		const JsonArray animationSettings = ledZone[F("animationSettings")].as<JsonArray>();
		for (uint8_t j = 0; j < ANIMATOR_NUM_ANIMATION_SETTINGS; j++)
		{
			config[i].animationSettings[j] = animationSettings[j].as<uint8_t>();
		}

		const JsonArray channelCurrents = ledZone[F("channelCurrents")].as<JsonArray>();
		for (uint8_t j = 0; j < 3; j++)
		{
			config[i].ledChannelCurrent[j] = channelCurrents[j].as<uint8_t>();
		}
	}

	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		TL::Configuration::setLedConfig(config[i], i);
	}

	const TL::Configuration::Error configSaveError = TL::Configuration::save();
	if (configSaveError == TL::Configuration::Error::ERROR_FILE_OPEN)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save LED configuration. The configuration file could not be opened."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save LED configuration. The configuration file could not be opened."));
		return;
	}
	else if (configSaveError == TL::Configuration::Error::ERROR_FILE_WRITE)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save LED configuration. The configuration file could not be written."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save LED configuration. The configuration file could not be written."));
		return;
	}
	else if (configSaveError != TL::Configuration::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save LED configuration."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save LED configuration."));
		return;
	}

	const TL::LedManager::Error ledManagerError = TL::LedManager::reloadAnimations();
	if (ledManagerError == TL::LedManager::Error::ERROR_INIT_LED_DRIVER)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialized LED driver for the current configuration."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(500, F("Failed to initialized LED driver for the current configuration."));
		return;
	}
	else if (ledManagerError == TL::LedManager::Error::ERROR_UNKNOWN_ANIMATOR_TYPE)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration. One of the animator types is unknown."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, F("Failed to apply LED configuration. One of the animator types is unknown."));
		return;
	}
	else if (ledManagerError == TL::LedManager::Error::ERROR_INVALID_FSEQ)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration. The selected fseq file is invalid."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, F("Failed to apply LED configuration. The selected fseq file is invalid."));
		return;
	}
	else if (ledManagerError == TL::LedManager::Error::ERROR_FILE_NOT_FOUND)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration. No fseq file with selected file id was found."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, F("Failed to apply LED configuration. No fseq file with selected file id was found."));
		return;
	}
	else if (ledManagerError == TL::LedManager::Error::ERROR_INVALID_LED_CONFIGURATION)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration. The current configurationn does not match the configuration of the fseq file."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, F("Failed to apply LED configuration. The current configurationn does not match the configuration of the fseq file."));
		return;
	}
	else if (ledManagerError != TL::LedManager::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration because of unknown reason"));
		TL::LedConfigurationEndpoint::sendSimpleResponse(500, F("Failed to apply LED configuration because of unknown reason"));
		return;
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("LED configuration saved. Sending the response."));
	TL::LedConfigurationEndpoint::sendSimpleResponse(200, F("Configuration saved! I am sure this will look great UwU !"));
}

/**
 * @brief Validate if a LED zone configuration is valid.
 * @param jsonObject json object holding the configuration for the LED zone
 * @param index index of the zone in the configuration
 * @return true when valid
 * @return false when invalid
 */
bool TL::LedConfigurationEndpoint::validateLedZone(const JsonObject &jsonObject, const uint8_t index)
{
	if (!jsonObject[F("ledPin")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"ledPin\" field at index ") + index + F(" must be of type \"uint8\"."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"ledPin\" field at index ") + index + F(" must be of type \"uint8\"."));
		return false;
	}

	if (!TL::LedConfigurationEndpoint::isValidPin(jsonObject[F("ledPin")].as<uint8_t>()))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"ledPin\" field at index ") + index + F(" must be a valid output pin."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"ledPin\" field at index ") + index + F(" must be a valid output pin."));
		return false;
	}

	if (!jsonObject[F("ledCount")].is<uint16_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"ledCount\" field at index ") + index + F(" must be of type \"uint16\"."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"ledCount\" field at index ") + index + F(" must be of type \"uint16\"."));
		return false;
	}

	if (!TL::LedConfigurationEndpoint::isInRange(jsonObject[F("ledCount")].as<uint16_t>(), 1L, LED_MAX_COUNT_PER_ZONE))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"ledCount\" field at index ") + index + F(" must be between 1 and ") + LED_MAX_COUNT_PER_ZONE + F("."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"ledCount\" field at index ") + index + F(" must be between 1 and ") + LED_MAX_COUNT_PER_ZONE + F("."));
		return false;
	}

	if (!jsonObject[F("type")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"type\" field at index ") + index + F(" must be of type \"uint8\"."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"type\" field at index ") + index + F(" must be of type \"uint8\"."));
		return false;
	}

	if (!TL::LedConfigurationEndpoint::isInRange(jsonObject[F("type")].as<uint8_t>(), 0L, 6L) && jsonObject[F("type")].as<uint8_t>() != 255)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"type\" field at index ") + index + F(" must be between 0 and 6."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"type\" field at index ") + index + F(" must be between 0 and 6."));
		return false;
	}

	if (!jsonObject[F("dataSource")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"dataSource\" field at index ") + index + F(" must be of type \"uint8\"."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"dataSource\" field at index ") + index + F(" must be of type \"uint8\"."));
		return false;
	}

	if (!TL::LedConfigurationEndpoint::isInRange(jsonObject[F("dataSource")].as<uint8_t>(), 0L, 21L))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"dataSource\" field at index ") + index + F(" must be between 0 and 21."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"dataSource\" field at index ") + index + F(" must be between 0 and 21."));
		return false;
	}

	if (!TL::MotionSensor::isInitialized() && TL::LedConfigurationEndpoint::isInRange(jsonObject[F("dataSource")].as<uint8_t>(), 2L, 18L))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"dataSource\" field at index ") + index + F(" is invalid. No motion sensor based data source can be selected because it is not available."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"dataSource\" field at index ") + index + F(" is invalid. No motion sensor based data source can be selected because it is not available."));
		return false;
	}

	if (!TL::AudioUnit::isInitialized() && TL::LedConfigurationEndpoint::isInRange(jsonObject[F("dataSource")].as<uint8_t>(), 19L, 21L))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"dataSource\" field at index ") + index + F(" is invalid. No audio based data source can be selected because the audio unit is not available."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"dataSource\" field at index ") + index + F(" is invalid. No audio based data source can be selected because the audio unit is not available."));
		return false;
	}

	if (!jsonObject[F("speed")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"speed\" field at index ") + index + F(" must be of type \"uint8\"."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"speed\" field at index ") + index + F(" must be of type \"uint8\"."));
		return false;
	}

	if (!jsonObject[F("offset")].is<uint16_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"offset\" field at index ") + index + F(" must be of type \"uint16\"."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"offset\" field at index ") + index + F(" must be of type \"uint16\"."));
		return false;
	}

	if (!jsonObject[F("brightness")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"brightness\" field at index ") + index + F(" must be of type \"uint8\"."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"brightness\" field at index ") + index + F(" must be of type \"uint8\"."));
		return false;
	}

	if (!jsonObject[F("reverse")].is<bool>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"reverse\" field at index ") + index + F(" must be of type \"bool\"."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"reverse\" field at index ") + index + F(" must be of type \"bool\"."));
		return false;
	}

	if (!jsonObject[F("fadeSpeed")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"fadeSpeed\" field at index ") + index + F(" must be of type \"uint8\"."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"fadeSpeed\" field at index ") + index + F(" must be of type \"uint8\"."));
		return false;
	}

	if (!jsonObject[F("ledVoltage")].is<float>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"ledVoltage\" field at index ") + index + F(" must be of type \"float\"."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"ledVoltage\" field at index ") + index + F(" must be of type \"float\"."));
		return false;
	}

	if (!jsonObject[F("animationSettings")].is<JsonArray>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"animationSettings\" field at index ") + index + F(" must be of type \"array\"."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"animationSettings\" field at index ") + index + F(" must be of type \"array\"."));
		return false;
	}

	if (jsonObject[F("animationSettings")].as<JsonArray>().size() != ANIMATOR_NUM_ANIMATION_SETTINGS)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"animationSettings\" array must have exactly ") + ANIMATOR_NUM_ANIMATION_SETTINGS + F(" elements."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"animationSettings\" array must have exactly ") + ANIMATOR_NUM_ANIMATION_SETTINGS + F(" elements."));
		return false;
	}

	for (uint8_t i = 0; i < ANIMATOR_NUM_ANIMATION_SETTINGS; i++)
	{
		if (!jsonObject[F("animationSettings")][i].is<uint8_t>())
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"animationSettings\" field at index ") + index + F(" must only contain elements of type \"uint8\"."));
			TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"animationSettings\" field at index ") + index + F(" must only contain elements of type \"uint8\"."));
			return false;
		}
	}

	if (!TL::LedConfigurationEndpoint::validateAnimationSettings(jsonObject[F("type")], jsonObject[F("animationSettings")]))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("One of the fields in the \"animationSettings\" array at index ") + index + F(" is invalid."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("One of the fields in the \"animationSettings\" array at index ") + index + F(" is invalid."));
		return false;
	}

	if (!jsonObject[F("channelCurrents")].is<JsonArray>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"channelCurrents\" field at index ") + index + F(" must be of type \"array\"."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"channelCurrents\" field at index ") + index + F(" must be of type \"array\"."));
		return false;
	}

	if (jsonObject[F("channelCurrents")].as<JsonArray>().size() != 3)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"channelCurrents\" array must have exactly ") + ANIMATOR_NUM_ANIMATION_SETTINGS + F(" elements."));
		TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"channelCurrents\" array must have exactly ") + ANIMATOR_NUM_ANIMATION_SETTINGS + F(" elements."));
		return false;
	}

	for (uint8_t i = 0; i < 3; i++)
	{
		if (!jsonObject[F("channelCurrents")][i].is<uint8_t>())
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"channelCurrents\" field at index ") + index + F(" must only contain elements of type \"uint8\"."));
			TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"channelCurrents\" field at index ") + index + F(" must only contain elements of type \"uint8\"."));
			return false;
		}
	}

	return true;
}

/**
 * @brief Calidate the animation settings based on the animatino type.
 * @param type type of the animation
 * @param jsonArray array holding the animation settings
 * @return true when valid
 * @return false when invalid
 */
bool TL::LedConfigurationEndpoint::validateAnimationSettings(const uint8_t type, const JsonArray &jsonArray)
{
	// Contains an array with the valid ranges for all animation settings
	// [0] = type
	// [1] = setting index
	// [2] = min value
	// [3] = max value
	const size_t numElements = 8;
	const uint8_t valueRanges[numElements][4] = {
		{0, 0, 0, 2},
		{1, 0, 0, 3},
		{2, 0, 0, 1},
		{4, 0, 0, 3},
		{5, 0, 0, 2},
		{5, 7, 0, 18},
		{6, 0, 0, 1},
		{6, 7, 0, 18}};

	for (size_t i = 0; i < numElements; i++)
	{
		if (valueRanges[i][0] != type)
		{
			continue;
		}

		if (!TL::LedConfigurationEndpoint::isInRange((long)jsonArray[valueRanges[i][1]], (long)valueRanges[i][2], (long)valueRanges[i][3]))
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"animationSettings\" value at index ") + valueRanges[i][1] + F(" is invalid."));
			TL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"animationSettings\" value at index ") + valueRanges[i][1] + F(" is invalid."));
			return false;
		}
	}

	return true;
}

/**
 * @brief Check if a pin number is a valid output pin for a zone.
 * @param pinNumber number of the pin
 * @return true when the pin number is valid
 * @return false when the pin number is invalid
 */
bool TL::LedConfigurationEndpoint::isValidPin(const uint8_t pinNumber)
{
	const uint8_t validLedPins[LED_NUM_ZONES] = LED_DEFAULT_OUTPUT_PINS;
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		if (validLedPins[i] == pinNumber)
		{
			return true;
		}
	}

	return false;
}

/**
 * @brief Check if a value is in range.
 * @param value value to check
 * @param min minimum value
 * @param max maximum value
 * @return true when the value is valid
 * @return false when the value is invalid
 */
bool TL::LedConfigurationEndpoint::isInRange(const long value, const long min, const long max)
{
	return value >= min && value <= max;
}

/**
 * @brief Check if a value is in range.
 * @param value value to check
 * @param min minimum value
 * @param max maximum value
 * @return true when the value is valid
 * @return false when the value is invalid
 */
bool TL::LedConfigurationEndpoint::isInRange(const float value, const float min, const float max)
{
	return value >= min && value <= max;
}