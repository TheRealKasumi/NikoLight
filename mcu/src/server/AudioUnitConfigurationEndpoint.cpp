/**
 * @file AudioUnitConfigurationEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to configure the NikoLight Audio Unit.
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
 * @brief Add all request handler for this {@link NL::RestEndpoint} to the {@link NL::WebServerManager}.
 */
void NL::AudioUnitConfigurationEndpoint::begin()
{
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/audio")).c_str(), http_method::HTTP_GET, NL::AudioUnitConfigurationEndpoint::getAudioUnitConfig);
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/audio")).c_str(), http_method::HTTP_PATCH, NL::AudioUnitConfigurationEndpoint::patchAudioUnitConfig);
}

/**
 * @brief Return the audio unit configuration to the client.
 */
void NL::AudioUnitConfigurationEndpoint::getAudioUnitConfig()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the audio unit configuration."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	DynamicJsonDocument jsonDoc(2048);
	const JsonObject config = jsonDoc.createNestedObject(F("audioUnitConfig"));
	config[F("noiseThreshold")] = NL::Configuration::getAudioUnitConfig().noiseThreshold;

	const JsonArray binConfigArray = config.createNestedArray(F("frequencyBinConfig"));
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		JsonObject binConfig = binConfigArray.createNestedObject();
		binConfig[F("start")] = NL::Configuration::getAudioUnitConfig().frequencyBandIndex[i].first;
		binConfig[F("end")] = NL::Configuration::getAudioUnitConfig().frequencyBandIndex[i].second;
	}

	const JsonArray peakDetectorArray = config.createNestedArray(F("peakDetectorConfig"));
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		JsonObject peakDetector = peakDetectorArray.createNestedObject();
		peakDetector[F("historySize")] = NL::Configuration::getAudioUnitConfig().peakDetectorConfig[i].historySize;
		peakDetector[F("threshold")] = NL::Configuration::getAudioUnitConfig().peakDetectorConfig[i].threshold;
		peakDetector[F("influence")] = NL::Configuration::getAudioUnitConfig().peakDetectorConfig[i].influence;
		peakDetector[F("noiseGate")] = NL::Configuration::getAudioUnitConfig().peakDetectorConfig[i].noiseGate;
	}

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	NL::AudioUnitConfigurationEndpoint::sendJsonDocument(200, F("Here you go."), jsonDoc);
}

/**
 * @brief Update the audio unit configuration.
 */
void NL::AudioUnitConfigurationEndpoint::patchAudioUnitConfig()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the audio unit configuration."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!NL::AudioUnit::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The audio unit was not initialized. Can not update configuration."));
		NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(500, F("The audio unit was not initialized. Can not update configuration."));
		return;
	}

	if (!NL::AudioUnitConfigurationEndpoint::webServer->hasHeader(F("content-type")) || NL::AudioUnitConfigurationEndpoint::webServer->header(F("content-type")) != F("application/json"))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/json\"."));
		NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, F("The content type must be \"application/json\"."));
		return;
	}

	if (!NL::AudioUnitConfigurationEndpoint::webServer->hasArg(F("plain")))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a valid json body with the audio unit configuration."));
		NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, F("There must be a valid json body with the audio unit configuration."));
		return;
	}

	const String body = NL::AudioUnitConfigurationEndpoint::webServer->arg(F("plain"));
	if (body.length() == 0 || body.length() > 2048)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 2048 bytes."));
		NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 2048 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(2048);
	if (!NL::AudioUnitConfigurationEndpoint::parseJsonDocument(jsonDoc, body))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body could not be parsed. The json is invalid."));
		NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, F("The body could not be parsed. The json is invalid."));
		return;
	}

	if (!jsonDoc[F("audioUnitConfig")].is<JsonObject>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The json must contain a \"audioUnitConfig\" object."));
		NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, F("The json must contain a \"audioUnitConfig\" object."));
		return;
	}

	const JsonObject configuration = jsonDoc[F("audioUnitConfig")].as<JsonObject>();
	if (!NL::AudioUnitConfigurationEndpoint::validateConfiguration(configuration))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The validation of the configuration failed."));
		return;
	}

	NL::Configuration::AudioUnitConfig config;
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

	NL::Configuration::setAudioUnitConfig(config);
	const NL::Configuration::Error configSaveError = NL::Configuration::save();
	if (configSaveError == NL::Configuration::Error::ERROR_FILE_OPEN)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save audio unit configuration. The configuration file could not be opened."));
		NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save audio unit configuration. The configuration file could not be opened."));
		return;
	}
	else if (configSaveError == NL::Configuration::Error::ERROR_FILE_WRITE)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save audio unit configuration. The configuration file could not be written."));
		NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save audio unit configuration. The configuration file could not be written."));
		return;
	}
	else if (configSaveError != NL::Configuration::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save audio unit configuration."));
		NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save audio unit configuration."));
		return;
	}

	// Convert the configuration and apply it to the audio unit
	NL::AudioUnit::AudioUnitConfig audioUnitConfig;
	audioUnitConfig.noiseThreshold = NL::Configuration::getAudioUnitConfig().noiseThreshold;
	audioUnitConfig.frequencyBandIndex.resize(AUDIO_UNIT_NUM_BANDS);
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		audioUnitConfig.frequencyBandIndex.at(i) = NL::Configuration::getAudioUnitConfig().frequencyBandIndex[i];
	}

	const NL::AudioUnit::Error audioConfigError = NL::AudioUnit::setAudioUnitConfig(audioUnitConfig);
	if (audioConfigError != NL::AudioUnit::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to apply the configuration to the audio unit."));
		NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(500, F("Failed to apply the configuration to the audio unit."));
		return;
	}

	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		const NL::AudioUnit::Error audioError = NL::AudioUnit::setPeakDetectorConfig(NL::Configuration::getAudioUnitConfig().peakDetectorConfig[i], i);
		if (audioError != NL::AudioUnit::Error::OK)
		{
			NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to apply the configuration to the audio unit."));
			NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(500, F("Failed to apply the configuration to the audio unit."));
			return;
		}
	}

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(200, F("Oki, audio unit configuration is updated."));
}

/**
 * @brief Validate if a audio unit configuration is valid.
 * @param jsonObject json object holding the configuration
 * @return true when valid
 * @return false when invalid
 */
bool NL::AudioUnitConfigurationEndpoint::validateConfiguration(const JsonObject &jsonObject)
{
	if (!jsonObject[F("noiseThreshold")].is<uint16_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"noiseThreshold\" field must be of type \"uint16\"."));
		NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, F("The \"noiseThreshold\" field must be of type \"uint16\"."));
		return false;
	}

	if (!NL::AudioUnitConfigurationEndpoint::isInRange(jsonObject[F("noiseThreshold")].as<uint16_t>(), 0, 4096))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"noiseThreshold\" field is invalid. It should be between 0 and 4096."));
		NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, F("The \"noiseThreshold\" field is invalid. It should be between 0 and 4096."));
		return false;
	}

	if (!jsonObject[F("frequencyBinConfig")].is<JsonArray>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"frequencyBinConfig\" field must be of type \"JsonArray\"."));
		NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, F("The \"frequencyBinConfig\" field must be of type \"JsonArray\"."));
		return false;
	}

	if (jsonObject[F("frequencyBinConfig")].as<JsonArray>().size() != AUDIO_UNIT_NUM_BANDS)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"frequencyBinConfig\" array must contain ") + AUDIO_UNIT_NUM_BANDS + F(" elements."));
		NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"frequencyBinConfig\" array must contain ") + AUDIO_UNIT_NUM_BANDS + F(" elements."));
		return false;
	}

	if (!jsonObject[F("peakDetectorConfig")].is<JsonArray>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"peakDetectorConfig\" field must be of type \"JsonArray\"."));
		NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, F("The \"peakDetectorConfig\" field must be of type \"JsonArray\"."));
		return false;
	}

	if (jsonObject[F("peakDetectorConfig")].as<JsonArray>().size() != AUDIO_UNIT_NUM_BANDS)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"peakDetectorConfig\" array must contain ") + AUDIO_UNIT_NUM_BANDS + F(" elements."));
		NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"peakDetectorConfig\" array must contain ") + AUDIO_UNIT_NUM_BANDS + F(" elements."));
		return false;
	}

	const JsonArray binConfigArray = jsonObject[F("frequencyBinConfig")].as<JsonArray>();
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		if (!binConfigArray[i][F("start")].is<uint16_t>())
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"start\" field at index ") + i + F(" must be of type \"uint16\"."));
			NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"start\" field at index ") + i + F(" must be of type \"uint16\"."));
			return false;
		}

		if (!NL::AudioUnitConfigurationEndpoint::isInRange(binConfigArray[i][F("start")].as<uint16_t>(), 0, 4096))
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"start\" field at index ") + i + F(" is invalid. It should be between 0 and 4096."));
			NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"start\" field at index ") + i + F(" is invalid. It should be between 0 and 4096."));
			return false;
		}

		if (!binConfigArray[i][F("end")].is<uint16_t>())
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"end\" field at index ") + i + F(" must be of type \"uint16\"."));
			NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"end\" field at index ") + i + F(" must be of type \"uint16\"."));
			return false;
		}

		if (!NL::AudioUnitConfigurationEndpoint::isInRange(binConfigArray[i][F("end")].as<uint16_t>(), 0, 4096))
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"end\" field at index ") + i + F(" is invalid. It should be between 0 and 4096."));
			NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"end\" field at index ") + i + F(" is invalid. It should be between 0 and 4096."));
			return false;
		}

		if (!NL::AudioUnitConfigurationEndpoint::validateMinMax(binConfigArray[i][F("start")].as<uint16_t>(), binConfigArray[i][F("end")].as<uint16_t>()))
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"start\" field at index ") + i + F(" must be smaller than the \"end\" field."));
			NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"start\" field at index ") + i + F(" must be smaller than the \"end\" field."));
			return false;
		}
	}

	const JsonArray peakDetectorArray = jsonObject[F("peakDetectorConfig")].as<JsonArray>();
	for (size_t i = 0; i < AUDIO_UNIT_NUM_BANDS; i++)
	{
		if (!peakDetectorArray[i][F("historySize")].is<uint16_t>())
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"historySize\" field at index ") + i + F(" must be of type \"uint16\"."));
			NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"historySize\" field at index ") + i + F(" must be of type \"uint16\"."));
			return false;
		}

		if (!NL::AudioUnitConfigurationEndpoint::isInRange(peakDetectorArray[i][F("historySize")].as<uint16_t>(), 0, 1024))
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"historySize\" field at index ") + i + F(" is invalid. It should be between 0 and 1024."));
			NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"historySize\" field at index ") + i + F(" is invalid. It should be between 0 and 1024."));
			return false;
		}

		if (!peakDetectorArray[i][F("threshold")].is<double>())
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"threshold\" field at index ") + i + F(" must be of type \"double\"."));
			NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"threshold\" field at index ") + i + F(" must be of type \"double\"."));
			return false;
		}

		if (!peakDetectorArray[i][F("influence")].is<double>())
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"influence\" field at index ") + i + F(" must be of type \"double\"."));
			NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"influence\" field at index ") + i + F(" must be of type \"double\"."));
			return false;
		}

		if (!peakDetectorArray[i][F("noiseGate")].is<double>())
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"noiseGate\" field at index ") + i + F(" must be of type \"double\"."));
			NL::AudioUnitConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"noiseGate\" field at index ") + i + F(" must be of type \"double\"."));
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
bool NL::AudioUnitConfigurationEndpoint::isInRange(const long value, const long min, const long max)
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
bool NL::AudioUnitConfigurationEndpoint::validateMinMax(const long min, const long max)
{
	return min < max;
}
