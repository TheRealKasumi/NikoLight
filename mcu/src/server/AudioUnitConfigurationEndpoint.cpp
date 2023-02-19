/**
 * @file AudioUnitConfigurationEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to configure the TesLight Audio Unit.
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
#include "server/AudioUnitConfigurationEndpoint.h"

/**
 * @brief Add all request handler for this {@link TL::RestEndpoint} to the {@link TL::WebServerManager}.
 */
void TL::AudioUnitConfigurationEndpoint::begin()
{
	TL::WebServerManager::addRequestHandler((getBaseUri() + F("config/audio")).c_str(), http_method::HTTP_GET, TL::AudioUnitConfigurationEndpoint::getAudioUnitConfig);
	TL::WebServerManager::addRequestHandler((getBaseUri() + F("config/audio")).c_str(), http_method::HTTP_POST, TL::AudioUnitConfigurationEndpoint::postAudioUnitConfig);
}

/**
 * @brief Return the audio unit configuration to the client.
 */
void TL::AudioUnitConfigurationEndpoint::getAudioUnitConfig()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the audio unit configuration."));
	if (!TL::Configuration::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TesLight configuration was not initialized. Can not access configuration."));
		TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(500, F("The TesLight configuration was not initialized. Can not access configuration."));
		return;
	}

	DynamicJsonDocument jsonDoc(2048);
	const JsonObject config = jsonDoc.createNestedObject(F("audioUnitConfig"));
	config[F("noiseThreshold")] = TL::Configuration::getAudioUnitConfig().noiseThreshold;

	const JsonArray binConfigArray = config.createNestedArray(F("frequencyBinConfig"));
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		JsonObject binConfig = binConfigArray.createNestedObject();
		binConfig[F("start")] = TL::Configuration::getAudioUnitConfig().frequencyBandIndex[i].first;
		binConfig[F("end")] = TL::Configuration::getAudioUnitConfig().frequencyBandIndex[i].second;
	}

	const JsonArray peakDetectorArray = config.createNestedArray(F("peakDetectorConfig"));
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		JsonObject peakDetector = peakDetectorArray.createNestedObject();
		peakDetector[F("historySize")] = TL::Configuration::getAudioUnitConfig().peakDetectorConfig[i].historySize;
		peakDetector[F("threshold")] = TL::Configuration::getAudioUnitConfig().peakDetectorConfig[i].threshold;
		peakDetector[F("influence")] = TL::Configuration::getAudioUnitConfig().peakDetectorConfig[i].influence;
		peakDetector[F("noiseGate")] = TL::Configuration::getAudioUnitConfig().peakDetectorConfig[i].noiseGate;
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::AudioUnitConfigurationEndpoint::sendJsonDocument(200, F("Here you go."), jsonDoc);
}

/**
 * @brief Receive the audio unit configuration sent by the client.
 */
void TL::AudioUnitConfigurationEndpoint::postAudioUnitConfig()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the audio unit configuration."));
	if (!TL::Configuration::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The TesLight configuration was not initialized. Can not access configuration."));
		TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(500, F("The TesLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!TL::AudioUnit::isInitialized())
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The audio unit was not initialized. Can not update configuration."));
		TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(500, F("The audio unit was not initialized. Can not update configuration."));
		return;
	}

	if (!TL::AudioUnitConfigurationEndpoint::webServer->hasHeader(F("content-type")) || TL::AudioUnitConfigurationEndpoint::webServer->header(F("content-type")) != F("application/json"))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/json\"."));
		TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, F("The content type must be \"application/json\"."));
		return;
	}

	if (!TL::AudioUnitConfigurationEndpoint::webServer->hasArg(F("plain")))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a valid json body with the audio unit configuration."));
		TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, F("There must be a valid json body with the audio unit configuration."));
		return;
	}

	const String body = TL::AudioUnitConfigurationEndpoint::webServer->arg(F("plain"));
	if (body.length() == 0 || body.length() > 2048)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 2048 bytes."));
		TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 2048 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(2048);
	if (!TL::AudioUnitConfigurationEndpoint::parseJsonDocument(jsonDoc, body))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body could not be parsed. The json is invalid."));
		TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, F("The body could not be parsed. The json is invalid."));
		return;
	}

	if (!jsonDoc[F("audioUnitConfig")].is<JsonObject>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The json must contain a \"audioUnitConfig\" object."));
		TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, F("The json must contain a \"audioUnitConfig\" object."));
		return;
	}

	const JsonObject configuration = jsonDoc[F("audioUnitConfig")].as<JsonObject>();
	if (!TL::AudioUnitConfigurationEndpoint::validateConfiguration(configuration))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The validation of the configuration failed."));
		return;
	}

	TL::Configuration::AudioUnitConfig config;
	config.noiseThreshold = configuration[F("noiseThreshold")].as<uint16_t>();

	const JsonArray binConfigArray = configuration[F("frequencyBinConfig")].as<JsonArray>();
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		config.frequencyBandIndex[i].first = binConfigArray[i][F("start")].as<uint16_t>();
		config.frequencyBandIndex[i].second = binConfigArray[i][F("end")].as<uint16_t>();
	}

	const JsonArray peakDetectorArray = configuration[F("peakDetectorConfig")].as<JsonArray>();
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		config.peakDetectorConfig[i].historySize = peakDetectorArray[i][F("historySize")].as<uint16_t>();
		config.peakDetectorConfig[i].threshold = peakDetectorArray[i][F("threshold")].as<double>();
		config.peakDetectorConfig[i].influence = peakDetectorArray[i][F("influence")].as<double>();
		config.peakDetectorConfig[i].noiseGate = peakDetectorArray[i][F("noiseGate")].as<double>();
	}

	TL::Configuration::setAudioUnitConfig(config);
	const TL::Configuration::Error configSaveError = TL::Configuration::save();
	if (configSaveError == TL::Configuration::Error::ERROR_FILE_OPEN)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save audio unit configuration. The configuration file could not be opened."));
		TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save audio unit configuration. The configuration file could not be opened."));
		return;
	}
	else if (configSaveError == TL::Configuration::Error::ERROR_FILE_WRITE)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save audio unit configuration. The configuration file could not be written."));
		TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save audio unit configuration. The configuration file could not be written."));
		return;
	}
	else if (configSaveError != TL::Configuration::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save audio unit configuration."));
		TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save audio unit configuration."));
		return;
	}

	// Convert the configuration and apply it to the audio unit
	TL::AudioUnit::AudioUnitConfig audioUnitConfig;
	audioUnitConfig.noiseThreshold = TL::Configuration::getAudioUnitConfig().noiseThreshold;
	audioUnitConfig.frequencyBandIndex.resize(AUDIO_UNIT_NUM_BANDS);
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		audioUnitConfig.frequencyBandIndex.at(i) = TL::Configuration::getAudioUnitConfig().frequencyBandIndex[i];
	}

	const TL::AudioUnit::Error audioConfigError = TL::AudioUnit::setAudioUnitConfig(audioUnitConfig);
	if (audioConfigError != TL::AudioUnit::Error::OK)
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to apply the configuration to the audio unit."));
		TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(500, F("Failed to apply the configuration to the audio unit."));
		return;
	}

	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		const TL::AudioUnit::Error audioError = TL::AudioUnit::setPeakDetectorConfig(TL::Configuration::getAudioUnitConfig().peakDetectorConfig[i], i);
		if (audioError != TL::AudioUnit::Error::OK)
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to apply the configuration to the audio unit."));
			TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(500, F("Failed to apply the configuration to the audio unit."));
			return;
		}
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(200, F("Oki, audio unit configuration is updated."));
}

/**
 * @brief Validate if a audio unit configuration is valid.
 * @param jsonObject json object holding the configuration
 * @return true when valid
 * @return false when invalid
 */
bool TL::AudioUnitConfigurationEndpoint::validateConfiguration(const JsonObject &jsonObject)
{
	if (!jsonObject[F("noiseThreshold")].is<uint16_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"noiseThreshold\" field must be of type \"uint16\"."));
		TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, F("The \"noiseThreshold\" field must be of type \"uint16\"."));
		return false;
	}

	if (!TL::AudioUnitConfigurationEndpoint::isInRange(jsonObject[F("noiseThreshold")].as<uint16_t>(), 0, 4096))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"noiseThreshold\" field is invalid. It should be between 0 and 4096."));
		TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, F("The \"noiseThreshold\" field is invalid. It should be between 0 and 4096."));
		return false;
	}

	if (!jsonObject[F("frequencyBinConfig")].is<JsonArray>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"frequencyBinConfig\" field must be of type \"JsonArray\"."));
		TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, F("The \"frequencyBinConfig\" field must be of type \"JsonArray\"."));
		return false;
	}

	if (jsonObject[F("frequencyBinConfig")].as<JsonArray>().size() != AUDIO_UNIT_NUM_BANDS)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"frequencyBinConfig\" array must contain ") + AUDIO_UNIT_NUM_BANDS + F(" elements."));
		TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"frequencyBinConfig\" array must contain ") + AUDIO_UNIT_NUM_BANDS + F(" elements."));
		return false;
	}

	if (!jsonObject[F("peakDetectorConfig")].is<JsonArray>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"peakDetectorConfig\" field must be of type \"JsonArray\"."));
		TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, F("The \"peakDetectorConfig\" field must be of type \"JsonArray\"."));
		return false;
	}

	if (jsonObject[F("peakDetectorConfig")].as<JsonArray>().size() != AUDIO_UNIT_NUM_BANDS)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"peakDetectorConfig\" array must contain ") + AUDIO_UNIT_NUM_BANDS + F(" elements."));
		TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"peakDetectorConfig\" array must contain ") + AUDIO_UNIT_NUM_BANDS + F(" elements."));
		return false;
	}

	const JsonArray binConfigArray = jsonObject[F("frequencyBinConfig")].as<JsonArray>();
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		if (!binConfigArray[i][F("start")].is<uint16_t>())
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"start\" field at index ") + i + F(" must be of type \"uint16\"."));
			TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"start\" field at index ") + i + F(" must be of type \"uint16\"."));
			return false;
		}

		if (!TL::AudioUnitConfigurationEndpoint::isInRange(binConfigArray[i][F("start")].as<uint16_t>(), 0, 4096))
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"start\" field at index ") + i + F(" is invalid. It should be between 0 and 4096."));
			TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"start\" field at index ") + i + F(" is invalid. It should be between 0 and 4096."));
			return false;
		}

		if (!binConfigArray[i][F("end")].is<uint16_t>())
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"end\" field at index ") + i + F(" must be of type \"uint16\"."));
			TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"end\" field at index ") + i + F(" must be of type \"uint16\"."));
			return false;
		}

		if (!TL::AudioUnitConfigurationEndpoint::isInRange(binConfigArray[i][F("end")].as<uint16_t>(), 0, 4096))
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"end\" field at index ") + i + F(" is invalid. It should be between 0 and 4096."));
			TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"end\" field at index ") + i + F(" is invalid. It should be between 0 and 4096."));
			return false;
		}

		if (!TL::AudioUnitConfigurationEndpoint::validateMinMax(binConfigArray[i][F("start")].as<uint16_t>(), binConfigArray[i][F("end")].as<uint16_t>()))
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"start\" field at index ") + i + F(" must be smaller than the \"end\" field."));
			TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"start\" field at index ") + i + F(" must be smaller than the \"end\" field."));
			return false;
		}
	}

	const JsonArray peakDetectorArray = jsonObject[F("peakDetectorConfig")].as<JsonArray>();
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		if (!peakDetectorArray[i][F("historySize")].is<uint16_t>())
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"historySize\" field at index ") + i + F(" must be of type \"uint16\"."));
			TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"historySize\" field at index ") + i + F(" must be of type \"uint16\"."));
			return false;
		}

		if (!TL::AudioUnitConfigurationEndpoint::isInRange(peakDetectorArray[i][F("historySize")].as<uint16_t>(), 0, 1024))
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"historySize\" field at index ") + i + F(" is invalid. It should be between 0 and 1024."));
			TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"historySize\" field at index ") + i + F(" is invalid. It should be between 0 and 1024."));
			return false;
		}

		if (!peakDetectorArray[i][F("threshold")].is<double>())
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"threshold\" field at index ") + i + F(" must be of type \"double\"."));
			TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"threshold\" field at index ") + i + F(" must be of type \"double\"."));
			return false;
		}

		if (!peakDetectorArray[i][F("influence")].is<double>())
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"influence\" field at index ") + i + F(" must be of type \"double\"."));
			TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"influence\" field at index ") + i + F(" must be of type \"double\"."));
			return false;
		}

		if (!peakDetectorArray[i][F("noiseGate")].is<double>())
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"noiseGate\" field at index ") + i + F(" must be of type \"double\"."));
			TL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"noiseGate\" field at index ") + i + F(" must be of type \"double\"."));
			return false;
		}
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
bool TL::AudioUnitConfigurationEndpoint::isInRange(const long value, const long min, const long max)
{
	return value >= min && value <= max;
}

/**
 * @brief Validate the min value to be smaller than the max value.
 * @param min min value
 * @param max max value
 * @return true when the min value is smaller than the max value
 * @return false when the min value is bigger than the max value
 */
bool TL::AudioUnitConfigurationEndpoint::validateMinMax(const long min, const long max)
{
	return min < max;
}
