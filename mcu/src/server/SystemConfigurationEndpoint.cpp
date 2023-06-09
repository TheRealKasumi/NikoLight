/**
 * @file SystemConfigurationEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to configure the system settings.
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
#include "server/SystemConfigurationEndpoint.h"

/**
 * @brief Add all request handler for this {@link NL::RestEndpoint} to the {@link NL::WebServerManager}.
 */
void NL::SystemConfigurationEndpoint::begin()
{
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/system")).c_str(), http_method::HTTP_GET, NL::SystemConfigurationEndpoint::getSystemConfig);
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("config/system")).c_str(), http_method::HTTP_PATCH, NL::SystemConfigurationEndpoint::patchSystemConfig);
}

/**
 * @brief Return the system configuration to the client.
 */
void NL::SystemConfigurationEndpoint::getSystemConfig()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the system configuration."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	DynamicJsonDocument jsonDoc(1024);
	const JsonObject config = jsonDoc.createNestedObject(F("systemConfig"));
	config[F("logLevel")] = NL::Configuration::getSystemConfig().logLevel;
	config[F("lightSensorMode")] = NL::Configuration::getSystemConfig().lightSensorMode;
	config[F("lightSensorThreshold")] = NL::Configuration::getSystemConfig().lightSensorThreshold;
	config[F("lightSensorMinAmbientBrightness")] = NL::Configuration::getSystemConfig().lightSensorMinAmbientBrightness;
	config[F("lightSensorMaxAmbientBrightness")] = NL::Configuration::getSystemConfig().lightSensorMaxAmbientBrightness;
	config[F("lightSensorMinLedBrightness")] = NL::Configuration::getSystemConfig().lightSensorMinLedBrightness;
	config[F("lightSensorMaxLedBrightness")] = NL::Configuration::getSystemConfig().lightSensorMaxLedBrightness;
	config[F("lightSensorDuration")] = NL::Configuration::getSystemConfig().lightSensorDuration;
	config[F("regulatorPowerLimit")] = NL::Configuration::getSystemConfig().regulatorPowerLimit;
	config[F("regulatorHighTemperature")] = NL::Configuration::getSystemConfig().regulatorHighTemperature;
	config[F("regulatorCutoffTemperature")] = NL::Configuration::getSystemConfig().regulatorCutoffTemperature;
	config[F("fanMode")] = NL::Configuration::getSystemConfig().fanMode;
	config[F("fanMinPwmValue")] = NL::Configuration::getSystemConfig().fanMinPwmValue;
	config[F("fanMaxPwmValue")] = NL::Configuration::getSystemConfig().fanMaxPwmValue;
	config[F("fanMinTemperature")] = NL::Configuration::getSystemConfig().fanMinTemperature;
	config[F("fanMaxTemperature")] = NL::Configuration::getSystemConfig().fanMaxTemperature;

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	NL::SystemConfigurationEndpoint::sendJsonDocument(200, F("Here you go."), jsonDoc);
}

/**
 * @brief Update the system configuration.
 */
void NL::SystemConfigurationEndpoint::patchSystemConfig()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the system configuration."));
	if (!NL::Configuration::isInitialized())
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("The NikoLight configuration was not initialized. Can not access configuration."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(500, F("The NikoLight configuration was not initialized. Can not access configuration."));
		return;
	}

	if (!NL::SystemConfigurationEndpoint::webServer->hasHeader(F("content-type")) || NL::SystemConfigurationEndpoint::webServer->header(F("content-type")) != F("application/json"))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/json\"."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The content type must be \"application/json\"."));
		return;
	}

	if (!NL::SystemConfigurationEndpoint::webServer->hasArg(F("plain")))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a valid json body with the system configuration."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("There must be a valid json body with the system configuration."));
		return;
	}

	const String body = NL::SystemConfigurationEndpoint::webServer->arg(F("plain"));
	if (body.length() == 0 || body.length() > 1024)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 1024 bytes."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 1024 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(1024);
	if (!NL::SystemConfigurationEndpoint::parseJsonDocument(jsonDoc, body))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body could not be parsed. The json is invalid."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The body could not be parsed. The json is invalid."));
		return;
	}

	if (!jsonDoc[F("systemConfig")].is<JsonObject>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The json must contain a \"systemConfig\" object."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The json must contain a \"systemConfig\" object."));
		return;
	}

	const JsonObject configuration = jsonDoc[F("systemConfig")].as<JsonObject>();
	if (!NL::SystemConfigurationEndpoint::validateConfiguration(configuration))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The validation of the configuration failed."));
		return;
	}

	NL::Configuration::SystemConfig config;
	config.logLevel = configuration[F("logLevel")].as<uint8_t>();
	config.lightSensorMode = configuration[F("lightSensorMode")].as<uint8_t>();
	config.lightSensorThreshold = configuration[F("lightSensorThreshold")].as<uint8_t>();
	config.lightSensorMinAmbientBrightness = configuration[F("lightSensorMinAmbientBrightness")].as<uint8_t>();
	config.lightSensorMaxAmbientBrightness = configuration[F("lightSensorMaxAmbientBrightness")].as<uint8_t>();
	config.lightSensorMinLedBrightness = configuration[F("lightSensorMinLedBrightness")].as<uint8_t>();
	config.lightSensorMaxLedBrightness = configuration[F("lightSensorMaxLedBrightness")].as<uint8_t>();
	config.lightSensorDuration = configuration[F("lightSensorDuration")].as<uint8_t>();
	config.regulatorPowerLimit = configuration[F("regulatorPowerLimit")].as<uint8_t>();
	config.regulatorHighTemperature = configuration[F("regulatorHighTemperature")].as<uint8_t>();
	config.regulatorCutoffTemperature = configuration[F("regulatorCutoffTemperature")].as<uint8_t>();
	config.fanMode = configuration[F("fanMode")].as<uint8_t>();
	config.fanMinPwmValue = configuration[F("fanMinPwmValue")].as<uint8_t>();
	config.fanMaxPwmValue = configuration[F("fanMaxPwmValue")].as<uint8_t>();
	config.fanMinTemperature = configuration[F("fanMinTemperature")].as<uint8_t>();
	config.fanMaxTemperature = configuration[F("fanMaxTemperature")].as<uint8_t>();

	NL::Configuration::setSystemConfig(config);
	const NL::Configuration::Error configSaveError = NL::Configuration::save();
	if (configSaveError == NL::Configuration::Error::ERROR_FILE_OPEN)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save system configuration. The configuration file could not be opened."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save system configuration. The configuration file could not be opened."));
		return;
	}
	else if (configSaveError == NL::Configuration::Error::ERROR_FILE_WRITE)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save system configuration. The configuration file could not be written."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save system configuration. The configuration file could not be written."));
		return;
	}
	else if (configSaveError != NL::Configuration::Error::OK)
	{
		NL::Logger::log(NL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save system configuration."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save system configuration."));
		return;
	}

	NL::Logger::setMinLogLevel((NL::Logger::LogLevel)NL::Configuration::getSystemConfig().logLevel);

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	NL::SystemConfigurationEndpoint::sendSimpleResponse(200, F("Oki, my system configuration is updated."));
}

/**
 * @brief Validate if a system configuration is valid.
 * @param jsonObject json object holding the configuration
 * @return true when valid
 * @return false when invalid
 */
bool NL::SystemConfigurationEndpoint::validateConfiguration(const JsonObject &jsonObject)
{
	if (!jsonObject[F("logLevel")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"logLevel\" field must be of type \"uint8\"."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"logLevel\" field must be of type \"uint8\"."));
		return false;
	}

	if (!NL::SystemConfigurationEndpoint::isInRange(jsonObject[F("logLevel")].as<uint8_t>(), 0, 3))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"logLevel\" field is invalid. It should be between 0 and 3."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"logLevel\" field is invalid. It should be between 0 and 3."));
		return false;
	}

	if (!jsonObject[F("lightSensorMode")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"lightSensorMode\" field must be of type \"uint8\"."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"lightSensorMode\" field must be of type \"uint8\"."));
		return false;
	}

	if (!NL::SystemConfigurationEndpoint::isInRange(jsonObject[F("lightSensorMode")].as<uint8_t>(), 0, 6))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"lightSensorMode\" field is invalid. It should be between 0 and 6."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"lightSensorMode\" field is invalid. It should be between 0 and 6."));
		return false;
	}

	if (!NL::AnalogInput::isInitialized() && NL::SystemConfigurationEndpoint::isInRange(jsonObject[F("lightSensorMode")].as<uint8_t>(), 2, 3))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"lightSensorMode\" field is invalid. Can not select ADC based mode because the ADC is not available."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"lightSensorMode\" field is invalid. Can not select ADC based mode because the ADC is not available."));
		return false;
	}

	if (!NL::BH1750::isInitialized() && NL::SystemConfigurationEndpoint::isInRange(jsonObject[F("lightSensorMode")].as<uint8_t>(), 4, 5))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"lightSensorMode\" field is invalid. Can not select BH1750 based mode because the BH1750 is not available."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"lightSensorMode\" field is invalid. Can not select BH1750 based mode because the BH1750 is not available."));
		return false;
	}

	if (!NL::MotionSensor::isInitialized() && jsonObject[F("lightSensorMode")].as<uint8_t>() == 6)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"lightSensorMode\" field is invalid. Can not select motion sensor based mode because the motion sensor is not available."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"lightSensorMode\" field is invalid. Can not select motion sensor based mode because the motion sensor is not available."));
		return false;
	}

	if (!jsonObject[F("lightSensorThreshold")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"lightSensorThreshold\" field must be of type \"uint8\"."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"lightSensorThreshold\" field must be of type \"uint8\"."));
		return false;
	}

	if (!jsonObject[F("lightSensorMinAmbientBrightness")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"lightSensorMinAmbientBrightness\" field must be of type \"uint8\"."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"lightSensorMinAmbientBrightness\" field must be of type \"uint8\"."));
		return false;
	}

	if (!jsonObject[F("lightSensorMaxAmbientBrightness")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"lightSensorMaxAmbientBrightness\" field must be of type \"uint8\"."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"lightSensorMaxAmbientBrightness\" field must be of type \"uint8\"."));
		return false;
	}

	if (!NL::SystemConfigurationEndpoint::validateMinMax(jsonObject[F("lightSensorMinAmbientBrightness")].as<uint8_t>(), jsonObject[F("lightSensorMaxAmbientBrightness")].as<uint8_t>()))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"lightSensorMinAmbientBrightness\" must be smaller than the \"lightSensorMaxAmbientBrightness\" value."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"lightSensorMinAmbientBrightness\" must be smaller than the \"lightSensorMaxAmbientBrightness\" value."));
		return false;
	}

	if (!jsonObject[F("lightSensorMinLedBrightness")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"lightSensorMinLedBrightness\" field must be of type \"uint8\"."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"lightSensorMinLedBrightness\" field must be of type \"uint8\"."));
		return false;
	}

	if (!jsonObject[F("lightSensorMaxLedBrightness")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"lightSensorMaxLedBrightness\" field must be of type \"uint8\"."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"lightSensorMaxLedBrightness\" field must be of type \"uint8\"."));
		return false;
	}

	if (!NL::SystemConfigurationEndpoint::validateMinMax(jsonObject[F("lightSensorMinLedBrightness")].as<uint8_t>(), jsonObject[F("lightSensorMaxLedBrightness")].as<uint8_t>()))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"lightSensorMinLedBrightness\" must be smaller than the \"lightSensorMaxLedBrightness\" value."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"lightSensorMinLedBrightness\" must be smaller than the \"lightSensorMaxLedBrightness\" value."));
		return false;
	}

	if (!jsonObject[F("lightSensorDuration")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"lightSensorDuration\" field must be of type \"uint8\"."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"lightSensorDuration\" field must be of type \"uint8\"."));
		return false;
	}

	if (!jsonObject[F("regulatorPowerLimit")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"regulatorPowerLimit\" field must be of type \"uint8\"."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"regulatorPowerLimit\" field must be of type \"uint8\"."));
		return false;
	}

	if (!NL::SystemConfigurationEndpoint::isInRange(jsonObject[F("regulatorPowerLimit")].as<uint8_t>(), 1, REGULATOR_POWER_LIMIT * REGULATOR_COUNT))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"regulatorPowerLimit\" field is invalid. It should be between 1 and ") + REGULATOR_POWER_LIMIT * REGULATOR_COUNT);
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"regulatorPowerLimit\" field is invalid. It should be between 1 and ") + REGULATOR_POWER_LIMIT * REGULATOR_COUNT);
		return false;
	}

	if (!jsonObject[F("regulatorHighTemperature")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"regulatorHighTemperature\" field must be of type \"uint8\"."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"regulatorHighTemperature\" field must be of type \"uint8\"."));
		return false;
	}

	if (!NL::SystemConfigurationEndpoint::isInRange(jsonObject[F("regulatorHighTemperature")].as<uint8_t>(), 60, REGULATOR_HIGH_TEMP_LIMIT))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"regulatorHighTemperature\" field is invalid. It should be between 60 and ") + REGULATOR_HIGH_TEMP_LIMIT + F("."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"regulatorHighTemperature\" field is invalid. It should be between 60 and ") + REGULATOR_HIGH_TEMP_LIMIT + F("."));
		return false;
	}

	if (!jsonObject[F("regulatorCutoffTemperature")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"regulatorCutoffTemperature\" field must be of type \"uint8\"."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"regulatorCutoffTemperature\" field must be of type \"uint8\"."));
		return false;
	}

	if (!NL::SystemConfigurationEndpoint::isInRange(jsonObject[F("regulatorCutoffTemperature")].as<uint8_t>(), 70, REGULATOR_CUT_OFF_TEMP_LIMIT))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"regulatorCutoffTemperature\" field is invalid. It should be between 70 and ") + REGULATOR_CUT_OFF_TEMP_LIMIT + F("."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"regulatorCutoffTemperature\" field is invalid. It should be between 70 and ") + REGULATOR_CUT_OFF_TEMP_LIMIT + F("."));
		return false;
	}

	if (!NL::SystemConfigurationEndpoint::validateMinMax(jsonObject[F("regulatorHighTemperature")].as<uint8_t>(), jsonObject[F("regulatorCutoffTemperature")].as<uint8_t>()))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"regulatorHighTemperature\" must be smaller than the \"regulatorCutoffTemperature\" value."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"regulatorHighTemperature\" must be smaller than the \"regulatorCutoffTemperature\" value."));
		return false;
	}

	if (!jsonObject[F("fanMode")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"fanMode\" field in configuration must be of type \"uint8\"."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"fanMode\" field in configuration must be of type \"uint8\"."));
		return false;
	}

	if (!NL::SystemConfigurationEndpoint::isInRange(jsonObject[F("fanMode")].as<uint8_t>(), 0, 5))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"fanMode\" field is invalid. It should be between 0 and 5."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"fanMode\" field is invalid. It should be between 0 and 5."));
		return false;
	}

	if (!jsonObject[F("fanMinPwmValue")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"fanMinPwmValue\" field must be of type \"uint8\"."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"fanMinPwmValue\" field must be of type \"uint8\"."));
		return false;
	}

	if (!jsonObject[F("fanMaxPwmValue")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"fanMaxPwmValue\" field must be of type \"uint8\"."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"fanMaxPwmValue\" field must be of type \"uint8\"."));
		return false;
	}

	if (!NL::SystemConfigurationEndpoint::validateMinMax(jsonObject[F("fanMinPwmValue")].as<uint8_t>(), jsonObject[F("fanMaxPwmValue")].as<uint8_t>()))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"fanMinPwmValue\" must be smaller than the \"fanMaxPwmValue\" value."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"fanMinPwmValue\" must be smaller than the \"fanMaxPwmValue\" value."));
		return false;
	}

	if (!jsonObject[F("fanMinTemperature")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"fanMinTemperature\" field must be of type \"uint8\"."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"fanMinTemperature\" field must be of type \"uint8\"."));
		return false;
	}

	if (!NL::SystemConfigurationEndpoint::isInRange(jsonObject[F("fanMinTemperature")].as<uint8_t>(), 45, 70))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"fanMinTemperature\" field is invalid. It should be between 45 and 70."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"fanMinTemperature\" field is invalid. It should be between 45 and 70."));
		return false;
	}

	if (!jsonObject[F("fanMaxTemperature")].is<uint8_t>())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"fanMaxTemperature\" field must be of type \"uint8\"."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"fanMaxTemperature\" field must be of type \"uint8\"."));
		return false;
	}

	if (!NL::SystemConfigurationEndpoint::isInRange(jsonObject[F("fanMaxTemperature")].as<uint8_t>(), 60, 90))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"fanMaxTemperature\" field is invalid. It should be between 60 and 90."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"fanMaxTemperature\" field is invalid. It should be between 60 and 90."));
		return false;
	}

	if (!NL::SystemConfigurationEndpoint::validateMinMax(jsonObject[F("fanMinTemperature")].as<uint8_t>(), jsonObject[F("fanMaxTemperature")].as<uint8_t>()))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The \"fanMinTemperature\" must be smaller than the \"fanMaxTemperature\" value."));
		NL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The \"fanMinTemperature\" must be smaller than the \"fanMaxTemperature\" value."));
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
bool NL::SystemConfigurationEndpoint::isInRange(const long value, const long min, const long max)
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
bool NL::SystemConfigurationEndpoint::validateMinMax(const long min, const long max)
{
	return min < max;
}
