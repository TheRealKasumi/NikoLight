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
 * @brief Add all request handler for this {@link NL::RestEndpoint} to the {@link NL::WebServerManager}.
 */
void NL::LedConfigurationEndpoint::begin()
{
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/led")).c_str(), http_method::HTTP_GET, NL::LedConfigurationEndpoint::getLedConfig);
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/led")).c_str(), http_method::HTTP_PATCH, NL::LedConfigurationEndpoint::patchLedConfig);
}

/**
 * @brief Return the LED configuration to the client.
 */
void NL::LedConfigurationEndpoint::getLedConfig()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the LED configuration."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	DynamicJsonDocument jsonDoc(5600);
	const JsonArray ledConfigArray = jsonDoc.createNestedArray(F("ledConfig"));
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		NL::Configuration::LedConfig ledConfig;
		NL::Configuration::getLedConfig(i, ledConfig);

		const JsonObject ledZone = ledConfigArray.createNestedObject();
		ledZone[F("ledPin")] = ledConfig.ledPin;
		ledZone[F("ledCount")] = ledConfig.ledCount;
		ledZone[F("type")] = ledConfig.type;
		ledZone[F("dataSource")] = ledConfig.dataSource;
		ledZone[F("speed")] = ledConfig.speed;
		ledZone[F("offset")] = ledConfig.offset;
		ledZone[F("brightness")] = ledConfig.brightness;
		ledZone[F("reverse")] = ledConfig.reverse;
		ledZone[F("fadeSpeed")] = ledConfig.fadeSpeed;
		ledZone[F("ledVoltage")] = ledConfig.ledVoltage;

		const JsonArray animationSettings = ledZone.createNestedArray(F("animationSettings"));
		for (uint8_t j = 0; j < ANIMATOR_NUM_ANIMATION_SETTINGS; j++)
		{
			animationSettings.add(ledConfig.animationSettings[j]);
		}

		const JsonArray channelCurrents = ledZone.createNestedArray(F("channelCurrents"));
		for (uint8_t j = 0; j < 3; j++)
		{
			channelCurrents.add(ledConfig.ledChannelCurrent[j]);
		}
	}

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	NL::LedConfigurationEndpoint::sendJsonDocument(200, F("Here is your LED configuration. Looks shiny :3 ."), jsonDoc);
}

/**
 * @brief Update the LED configuration.
 */
void NL::LedConfigurationEndpoint::patchLedConfig()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the LED configuration."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!NL::LedConfigurationEndpoint::webServer->hasHeader(F("content-type")) || NL::LedConfigurationEndpoint::webServer->header(F("content-type")) != F("application/json"))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/json\"."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, F("The content type must be \"application/json\"."));
		return;
	}

	if (!NL::LedConfigurationEndpoint::webServer->hasArg(F("plain")))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a valid json body with the LED configuration."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, F("There must be a valid json body with the LED configuration."));
		return;
	}

	const String body = NL::LedConfigurationEndpoint::webServer->arg(F("plain"));
	if (body.length() == 0 || body.length() > 4096)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 4096 bytes."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 4096 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(5600);
	if (!NL::LedConfigurationEndpoint::parseJsonDocument(jsonDoc, body))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body could not be parsed. The json is invalid."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, F("The body could not be parsed. The json is invalid."));
		return;
	}

	if (!jsonDoc[F("ledConfig")].is<JsonArray>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The json must contain a \"ledConfig\" array."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, F("The json must contain a \"ledConfig\" array."));
		return;
	}

	const JsonArray ledConfig = jsonDoc[F("ledConfig")].as<JsonArray>();
	if (ledConfig.size() != LED_NUM_ZONES)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"ledConfig\" must contain exactly ") + LED_NUM_ZONES + F(" zones."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"ledConfig\" must contain exactly ") + LED_NUM_ZONES + F(" zones."));
		return;
	}

	NL::Configuration::LedConfig config[LED_NUM_ZONES];
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		const JsonObject ledZone = ledConfig[i];
		if (!NL::LedConfigurationEndpoint::validateLedZone(ledZone, i))
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The validation of the LED configuration failed."));
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
		NL::Configuration::setLedConfig(i, config[i]);
	}

	const NL::Configuration::Error configSaveError = NL::Configuration::save();
	if (configSaveError == NL::Configuration::Error::ERROR_FILE_OPEN)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save LED configuration. The configuration file could not be opened."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save LED configuration. The configuration file could not be opened."));
		return;
	}
	else if (configSaveError == NL::Configuration::Error::ERROR_FILE_WRITE)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save LED configuration. The configuration file could not be written."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save LED configuration. The configuration file could not be written."));
		return;
	}
	else if (configSaveError != NL::Configuration::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save LED configuration."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save LED configuration."));
		return;
	}

	const NL::LedManager::Error ledManagerError = NL::LedManager::reloadAnimations();
	if (ledManagerError == NL::LedManager::Error::ERROR_INIT_LED_DRIVER)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialized LED driver for the current configuration."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(500, F("Failed to initialized LED driver for the current configuration."));
		return;
	}
	else if (ledManagerError == NL::LedManager::Error::ERROR_UNKNOWN_ANIMATOR_TYPE)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration. One of the animator types is unknown."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, F("Failed to apply LED configuration. One of the animator types is unknown."));
		return;
	}
	else if (ledManagerError == NL::LedManager::Error::ERROR_INVALID_FSEQ)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration. The selected fseq file is invalid."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, F("Failed to apply LED configuration. The selected fseq file is invalid."));
		return;
	}
	else if (ledManagerError == NL::LedManager::Error::ERROR_FILE_NOT_FOUND)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration. No fseq file with selected file id was found."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, F("Failed to apply LED configuration. No fseq file with selected file id was found."));
		return;
	}
	else if (ledManagerError == NL::LedManager::Error::ERROR_INVALID_LED_CONFIGURATION)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration. The current configurationn does not match the configuration of the fseq file."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, F("Failed to apply LED configuration. The current configurationn does not match the configuration of the fseq file."));
		return;
	}
	else if (ledManagerError != NL::LedManager::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply LED configuration because of unknown reason"));
		NL::LedConfigurationEndpoint::sendSimpleResponse(500, F("Failed to apply LED configuration because of unknown reason"));
		return;
	}

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("LED configuration saved. Sending the response."));
	NL::LedConfigurationEndpoint::sendSimpleResponse(200, F("Configuration saved! I am sure this will look great UwU !"));
}

/**
 * @brief Validate if a LED zone configuration is valid.
 * @param jsonObject json object holding the configuration for the LED zone
 * @param index index of the zone in the configuration
 * @return true when valid
 * @return false when invalid
 */
bool NL::LedConfigurationEndpoint::validateLedZone(const JsonObject &jsonObject, const uint8_t index)
{
	if (!jsonObject[F("ledPin")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"ledPin\" field at index ") + index + F(" must be of type \"uint8\"."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"ledPin\" field at index ") + index + F(" must be of type \"uint8\"."));
		return false;
	}

	if (!NL::LedConfigurationEndpoint::isValidPin(jsonObject[F("ledPin")].as<uint8_t>()))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"ledPin\" field at index ") + index + F(" must be a valid output pin."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"ledPin\" field at index ") + index + F(" must be a valid output pin."));
		return false;
	}

	if (!jsonObject[F("ledCount")].is<uint16_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"ledCount\" field at index ") + index + F(" must be of type \"uint16\"."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"ledCount\" field at index ") + index + F(" must be of type \"uint16\"."));
		return false;
	}

	if (!NL::LedConfigurationEndpoint::isInRange(jsonObject[F("ledCount")].as<uint16_t>(), 1L, LED_MAX_COUNT_PER_ZONE))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"ledCount\" field at index ") + index + F(" must be between 1 and ") + LED_MAX_COUNT_PER_ZONE + F("."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"ledCount\" field at index ") + index + F(" must be between 1 and ") + LED_MAX_COUNT_PER_ZONE + F("."));
		return false;
	}

	if (!jsonObject[F("type")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"type\" field at index ") + index + F(" must be of type \"uint8\"."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"type\" field at index ") + index + F(" must be of type \"uint8\"."));
		return false;
	}

	if (!NL::LedConfigurationEndpoint::isInRange(jsonObject[F("type")].as<uint8_t>(), 0L, 8L) && jsonObject[F("type")].as<uint8_t>() != 255)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"type\" field at index ") + index + F(" must be between 0 and 8."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"type\" field at index ") + index + F(" must be between 0 and 8."));
		return false;
	}

	if (!jsonObject[F("dataSource")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"dataSource\" field at index ") + index + F(" must be of type \"uint8\"."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"dataSource\" field at index ") + index + F(" must be of type \"uint8\"."));
		return false;
	}

	if (!NL::LedConfigurationEndpoint::isInRange(jsonObject[F("dataSource")].as<uint8_t>(), 0L, 21L))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"dataSource\" field at index ") + index + F(" must be between 0 and 21."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"dataSource\" field at index ") + index + F(" must be between 0 and 21."));
		return false;
	}

	if (!NL::MotionSensor::isInitialized() && NL::LedConfigurationEndpoint::isInRange(jsonObject[F("dataSource")].as<uint8_t>(), 2L, 18L))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"dataSource\" field at index ") + index + F(" is invalid. No motion sensor based data source can be selected because it is not available."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"dataSource\" field at index ") + index + F(" is invalid. No motion sensor based data source can be selected because it is not available."));
		return false;
	}

	if (!NL::AudioUnit::isInitialized() && NL::LedConfigurationEndpoint::isInRange(jsonObject[F("dataSource")].as<uint8_t>(), 19L, 21L))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"dataSource\" field at index ") + index + F(" is invalid. No audio based data source can be selected because the audio unit is not available."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"dataSource\" field at index ") + index + F(" is invalid. No audio based data source can be selected because the audio unit is not available."));
		return false;
	}

	if (!jsonObject[F("speed")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"speed\" field at index ") + index + F(" must be of type \"uint8\"."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"speed\" field at index ") + index + F(" must be of type \"uint8\"."));
		return false;
	}

	if (!jsonObject[F("offset")].is<uint16_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"offset\" field at index ") + index + F(" must be of type \"uint16\"."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"offset\" field at index ") + index + F(" must be of type \"uint16\"."));
		return false;
	}

	if (!jsonObject[F("brightness")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"brightness\" field at index ") + index + F(" must be of type \"uint8\"."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"brightness\" field at index ") + index + F(" must be of type \"uint8\"."));
		return false;
	}

	if (!jsonObject[F("reverse")].is<bool>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"reverse\" field at index ") + index + F(" must be of type \"bool\"."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"reverse\" field at index ") + index + F(" must be of type \"bool\"."));
		return false;
	}

	if (!jsonObject[F("fadeSpeed")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"fadeSpeed\" field at index ") + index + F(" must be of type \"uint8\"."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"fadeSpeed\" field at index ") + index + F(" must be of type \"uint8\"."));
		return false;
	}

	if (!jsonObject[F("ledVoltage")].is<float>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"ledVoltage\" field at index ") + index + F(" must be of type \"float\"."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"ledVoltage\" field at index ") + index + F(" must be of type \"float\"."));
		return false;
	}

	if (!jsonObject[F("animationSettings")].is<JsonArray>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"animationSettings\" field at index ") + index + F(" must be of type \"array\"."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"animationSettings\" field at index ") + index + F(" must be of type \"array\"."));
		return false;
	}

	if (jsonObject[F("animationSettings")].as<JsonArray>().size() != ANIMATOR_NUM_ANIMATION_SETTINGS)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"animationSettings\" array must have exactly ") + ANIMATOR_NUM_ANIMATION_SETTINGS + F(" elements."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"animationSettings\" array must have exactly ") + ANIMATOR_NUM_ANIMATION_SETTINGS + F(" elements."));
		return false;
	}

	for (uint8_t i = 0; i < ANIMATOR_NUM_ANIMATION_SETTINGS; i++)
	{
		if (!jsonObject[F("animationSettings")][i].is<uint8_t>())
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"animationSettings\" field at index ") + index + F(" must only contain elements of type \"uint8\"."));
			NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"animationSettings\" field at index ") + index + F(" must only contain elements of type \"uint8\"."));
			return false;
		}
	}

	if (!NL::LedConfigurationEndpoint::validateAnimationSettings(jsonObject[F("type")], jsonObject[F("animationSettings")]))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("One of the fields in the \"animationSettings\" array at index ") + index + F(" is invalid."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("One of the fields in the \"animationSettings\" array at index ") + index + F(" is invalid."));
		return false;
	}

	if (!jsonObject[F("channelCurrents")].is<JsonArray>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"channelCurrents\" field at index ") + index + F(" must be of type \"array\"."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"channelCurrents\" field at index ") + index + F(" must be of type \"array\"."));
		return false;
	}

	if (jsonObject[F("channelCurrents")].as<JsonArray>().size() != 3)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"channelCurrents\" array must have exactly ") + ANIMATOR_NUM_ANIMATION_SETTINGS + F(" elements."));
		NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"channelCurrents\" array must have exactly ") + ANIMATOR_NUM_ANIMATION_SETTINGS + F(" elements."));
		return false;
	}

	for (uint8_t i = 0; i < 3; i++)
	{
		if (!jsonObject[F("channelCurrents")][i].is<uint8_t>())
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"channelCurrents\" field at index ") + index + F(" must only contain elements of type \"uint8\"."));
			NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"channelCurrents\" field at index ") + index + F(" must only contain elements of type \"uint8\"."));
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
bool NL::LedConfigurationEndpoint::validateAnimationSettings(const uint8_t type, const JsonArray &jsonArray)
{
	// Contains an array with the valid ranges for all animation settings
	// [0] = type
	// [1] = setting index
	// [2] = min value
	// [3] = max value
	const size_t numElements = 9;
	const uint8_t valueRanges[numElements][4] = {
		{0, 0, 0, 2},
		{1, 0, 0, 3},
		{2, 0, 0, 1},
		{4, 0, 0, 3},
		{5, 0, 0, 2},
		{5, 7, 0, 18},
		{6, 0, 0, 1},
		{6, 7, 0, 18},
		{7, 0, 0, 1}};

	for (size_t i = 0; i < numElements; i++)
	{
		if (valueRanges[i][0] != type)
		{
			continue;
		}

		if (!NL::LedConfigurationEndpoint::isInRange((long)jsonArray[valueRanges[i][1]], (long)valueRanges[i][2], (long)valueRanges[i][3]))
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"animationSettings\" value at index ") + valueRanges[i][1] + F(" is invalid."));
			NL::LedConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"animationSettings\" value at index ") + valueRanges[i][1] + F(" is invalid."));
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
bool NL::LedConfigurationEndpoint::isValidPin(const uint8_t pinNumber)
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
bool NL::LedConfigurationEndpoint::isInRange(const long value, const long min, const long max)
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
bool NL::LedConfigurationEndpoint::isInRange(const float value, const float min, const float max)
{
	return value >= min && value <= max;
}