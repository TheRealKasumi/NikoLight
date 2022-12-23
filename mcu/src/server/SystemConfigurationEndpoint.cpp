/**
 * @file SystemConfigurationEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to configure the system settings.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "server/SystemConfigurationEndpoint.h"

// Initialize
TL::Configuration *TL::SystemConfigurationEndpoint::configuration = nullptr;
std::function<bool()> TL::SystemConfigurationEndpoint::configChangedCallback = nullptr;

/**
 * @brief Add all request handler for this {@link TL::RestEndpoint} to the {@link TL::WebServerManager}.
 */
void TL::SystemConfigurationEndpoint::begin(TL::Configuration *_configuration, std::function<bool()> _configChangedCallback)
{
	TL::SystemConfigurationEndpoint::configuration = _configuration;
	TL::SystemConfigurationEndpoint::configChangedCallback = _configChangedCallback;
	TL::SystemConfigurationEndpoint::webServerManager->addRequestHandler((getBaseUri() + F("config/system")).c_str(), http_method::HTTP_GET, TL::SystemConfigurationEndpoint::getSystemConfig);
	TL::SystemConfigurationEndpoint::webServerManager->addRequestHandler((getBaseUri() + F("config/system")).c_str(), http_method::HTTP_POST, TL::SystemConfigurationEndpoint::postSystemConfig);
}

/**
 * @brief Return the system configuration to the client as binary data.
 */
void TL::SystemConfigurationEndpoint::getSystemConfig()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the system configuration."));

	DynamicJsonDocument jsonDoc(1024);
	JsonObject config = jsonDoc.createNestedObject(F("systemConfig"));
	config[F("logLevel")] = TL::SystemConfigurationEndpoint::configuration->getSystemConfig().logLevel;
	config[F("lightSensorMode")] = TL::SystemConfigurationEndpoint::configuration->getSystemConfig().lightSensorMode;
	config[F("lightSensorThreshold")] = TL::SystemConfigurationEndpoint::configuration->getSystemConfig().lightSensorThreshold;
	config[F("lightSensorMinAmbientBrightness")] = TL::SystemConfigurationEndpoint::configuration->getSystemConfig().lightSensorMinAmbientBrightness;
	config[F("lightSensorMaxAmbientBrightness")] = TL::SystemConfigurationEndpoint::configuration->getSystemConfig().lightSensorMaxAmbientBrightness;
	config[F("lightSensorMinLedBrightness")] = TL::SystemConfigurationEndpoint::configuration->getSystemConfig().lightSensorMinLedBrightness;
	config[F("lightSensorMaxLedBrightness")] = TL::SystemConfigurationEndpoint::configuration->getSystemConfig().lightSensorMaxLedBrightness;
	config[F("lightSensorDuration")] = TL::SystemConfigurationEndpoint::configuration->getSystemConfig().lightSensorDuration;
	config[F("regulatorPowerLimit")] = TL::SystemConfigurationEndpoint::configuration->getSystemConfig().regulatorPowerLimit;
	config[F("regulatorHighTemperature")] = TL::SystemConfigurationEndpoint::configuration->getSystemConfig().regulatorHighTemperature;
	config[F("regulatorCutoffTemperature")] = TL::SystemConfigurationEndpoint::configuration->getSystemConfig().regulatorCutoffTemperature;
	config[F("fanMinPwmValue")] = TL::SystemConfigurationEndpoint::configuration->getSystemConfig().fanMinPwmValue;
	config[F("fanMaxPwmValue")] = TL::SystemConfigurationEndpoint::configuration->getSystemConfig().fanMaxPwmValue;
	config[F("fanMinTemperature")] = TL::SystemConfigurationEndpoint::configuration->getSystemConfig().fanMinTemperature;
	config[F("fanMaxTemperature")] = TL::SystemConfigurationEndpoint::configuration->getSystemConfig().fanMaxTemperature;

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::SystemConfigurationEndpoint::sendJsonDocument(200, F("Here you go."), jsonDoc);
}

/**
 * @brief Receive the system configuration sent by the client.
 */
void TL::SystemConfigurationEndpoint::postSystemConfig()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the system configuration."));

	if (!TL::SystemConfigurationEndpoint::webServer->hasHeader(F("content-type")) || TL::SystemConfigurationEndpoint::webServer->header(F("content-type")) != F("application/json"))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/json\"."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The content type must be \"application/json\"."));
		return;
	}

	if (!TL::SystemConfigurationEndpoint::webServer->hasArg(F("plain")))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There most be a valid json body with the system configuration."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("There most be a valid json body with the system configuration."));
		return;
	}

	const String body = TL::SystemConfigurationEndpoint::webServer->arg(F("plain"));
	if (body.length() == 0 || body.length() > 1024)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body must not be empty and the maximum length is 1024 bytes."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The body must not be empty and the maximum length is 1024 bytes."));
		return;
	}

	DynamicJsonDocument jsonDoc(1024);
	if (!TL::SystemConfigurationEndpoint::parseJsonDocument(jsonDoc, body))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The body could not be parsed. The json is invalid."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The body could not be parsed. The json is invalid."));
		return;
	}

	if (!jsonDoc[F("systemConfig")].is<JsonObject>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The json must contain the \"systemConfig\" object."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, F("The json must contain the \"systemConfig\" object."));
		return;
	}

	JsonObject configuration = jsonDoc[F("systemConfig")].as<JsonObject>();
	if (!TL::SystemConfigurationEndpoint::validateConfiguration(configuration))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The validation of the configuration failed."));
		return;
	}

	TL::Configuration::SystemConfig config;
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
	config.fanMinPwmValue = configuration[F("fanMinPwmValue")].as<uint8_t>();
	config.fanMaxPwmValue = configuration[F("fanMaxPwmValue")].as<uint8_t>();
	config.fanMinTemperature = configuration[F("fanMinTemperature")].as<uint8_t>();
	config.fanMaxTemperature = configuration[F("fanMaxTemperature")].as<uint8_t>();

	TL::SystemConfigurationEndpoint::configuration->setSystemConfig(config);
	if (TL::SystemConfigurationEndpoint::configuration->save())
	{
		if (TL::SystemConfigurationEndpoint::configChangedCallback)
		{
			if (!TL::SystemConfigurationEndpoint::configChangedCallback())
			{
				TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to apply the system configuration. The callback function returned with an error."));
				TL::SystemConfigurationEndpoint::sendSimpleResponse(500, F("Failed to apply the system configuration."));
				return;
			}
		}
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save system configuration."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(500, F("Failed to save system configuration."));
		return;
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::SystemConfigurationEndpoint::sendSimpleResponse(200, F("Oki, my system configuration is updated."));
}

/**
 * @brief Validate if a system configuration is valid.
 * @param jsonObject json object holding the configuration
 * @return true when valid
 * @return false when invalid
 */
bool TL::SystemConfigurationEndpoint::validateConfiguration(const JsonObject &jsonObject)
{
	if (!jsonObject[F("logLevel")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"logLevel\" field in configuration must be of type \"uint8\"."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"logLevel\" field in configuration must be of type \"uint8\"."));
		return false;
	}

	if (!TL::SystemConfigurationEndpoint::isInRange(jsonObject[F("logLevel")].as<uint8_t>(), 0, 3))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"logLevel\" field is invalid. It should be between 0 and 3."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"logLevel\" field is invalid. It should be between 0 and 3."));
		return false;
	}

	if (!jsonObject[F("lightSensorMode")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"lightSensorMode\" field in configuration must be of type \"uint8\"."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"lightSensorMode\" field in configuration must be of type \"uint8\"."));
		return false;
	}

	if (!TL::SystemConfigurationEndpoint::isInRange(jsonObject[F("lightSensorMode")].as<uint8_t>(), 0, 6))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"lightSensorMode\" field is invalid. It should be between 0 and 6."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"lightSensorMode\" field is invalid. It should be between 0 and 6."));
		return false;
	}

	if (!jsonObject[F("lightSensorThreshold")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"lightSensorThreshold\" field in configuration must be of type \"uint8\"."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"lightSensorThreshold\" field in configuration must be of type \"uint8\"."));
		return false;
	}

	if (!jsonObject[F("lightSensorMinAmbientBrightness")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"lightSensorMinAmbientBrightness\" field in configuration must be of type \"uint8\"."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"lightSensorMinAmbientBrightness\" field in configuration must be of type \"uint8\"."));
		return false;
	}

	if (!jsonObject[F("lightSensorMaxAmbientBrightness")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"lightSensorMaxAmbientBrightness\" field in configuration must be of type \"uint8\"."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"lightSensorMaxAmbientBrightness\" field in configuration must be of type \"uint8\"."));
		return false;
	}

	if (!TL::SystemConfigurationEndpoint::validateMinMax(jsonObject[F("lightSensorMinAmbientBrightness")].as<uint8_t>(), jsonObject[F("lightSensorMaxAmbientBrightness")].as<uint8_t>()))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"lightSensorMinAmbientBrightness\" must be smaller than the \"lightSensorMaxAmbientBrightness\" value."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"lightSensorMinAmbientBrightness\" must be smaller than the \"lightSensorMaxAmbientBrightness\" value."));
		return false;
	}

	if (!jsonObject[F("lightSensorMinLedBrightness")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"lightSensorMinLedBrightness\" field in configuration must be of type \"uint8\"."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"lightSensorMinLedBrightness\" field in configuration must be of type \"uint8\"."));
		return false;
	}

	if (!jsonObject[F("lightSensorMaxLedBrightness")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"lightSensorMaxLedBrightness\" field in configuration must be of type \"uint8\"."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"lightSensorMaxLedBrightness\" field in configuration must be of type \"uint8\"."));
		return false;
	}

	if (!TL::SystemConfigurationEndpoint::validateMinMax(jsonObject[F("lightSensorMinLedBrightness")].as<uint8_t>(), jsonObject[F("lightSensorMaxLedBrightness")].as<uint8_t>()))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"lightSensorMinLedBrightness\" must be smaller than the \"lightSensorMaxLedBrightness\" value."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"lightSensorMinLedBrightness\" must be smaller than the \"lightSensorMaxLedBrightness\" value."));
		return false;
	}

	if (!jsonObject[F("lightSensorDuration")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"lightSensorDuration\" field in configuration must be of type \"uint8\"."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"lightSensorDuration\" field in configuration must be of type \"uint8\"."));
		return false;
	}

	if (!jsonObject[F("regulatorPowerLimit")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"regulatorPowerLimit\" field in configuration must be of type \"uint8\"."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"regulatorPowerLimit\" field in configuration must be of type \"uint8\"."));
		return false;
	}

	if (!TL::SystemConfigurationEndpoint::isInRange(jsonObject[F("regulatorPowerLimit")].as<uint8_t>(), 1, 30))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"regulatorPowerLimit\" field is invalid. It should be between 1 and 30."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"regulatorPowerLimit\" field is invalid. It should be between 1 and 30."));
		return false;
	}

	if (!jsonObject[F("regulatorHighTemperature")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"regulatorHighTemperature\" field in configuration must be of type \"uint8\"."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"regulatorHighTemperature\" field in configuration must be of type \"uint8\"."));
		return false;
	}

	if (!TL::SystemConfigurationEndpoint::isInRange(jsonObject[F("regulatorHighTemperature")].as<uint8_t>(), 60, 85))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"regulatorHighTemperature\" field is invalid. It should be between 60 and 85."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"regulatorHighTemperature\" field is invalid. It should be between 60 and 85."));
		return false;
	}

	if (!jsonObject[F("regulatorCutoffTemperature")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"regulatorCutoffTemperature\" field in configuration must be of type \"uint8\"."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"regulatorCutoffTemperature\" field in configuration must be of type \"uint8\"."));
		return false;
	}

	if (!TL::SystemConfigurationEndpoint::isInRange(jsonObject[F("regulatorCutoffTemperature")].as<uint8_t>(), 70, 95))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"regulatorCutoffTemperature\" field is invalid. It should be between 70 and 95."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"regulatorCutoffTemperature\" field is invalid. It should be between 70 and 95."));
		return false;
	}

	if (!TL::SystemConfigurationEndpoint::validateMinMax(jsonObject[F("regulatorHighTemperature")].as<uint8_t>(), jsonObject[F("regulatorCutoffTemperature")].as<uint8_t>()))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"regulatorHighTemperature\" must be smaller than the \"regulatorCutoffTemperature\" value."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"regulatorHighTemperature\" must be smaller than the \"regulatorCutoffTemperature\" value."));
		return false;
	}

	if (!jsonObject[F("fanMinPwmValue")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"fanMinPwmValue\" field in configuration must be of type \"uint8\"."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"fanMinPwmValue\" field in configuration must be of type \"uint8\"."));
		return false;
	}

	if (!jsonObject[F("fanMaxPwmValue")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"fanMaxPwmValue\" field in configuration must be of type \"uint8\"."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"fanMaxPwmValue\" field in configuration must be of type \"uint8\"."));
		return false;
	}

	if (!TL::SystemConfigurationEndpoint::validateMinMax(jsonObject[F("fanMinPwmValue")].as<uint8_t>(), jsonObject[F("fanMaxPwmValue")].as<uint8_t>()))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"fanMinPwmValue\" must be smaller than the \"fanMaxPwmValue\" value."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"fanMinPwmValue\" must be smaller than the \"fanMaxPwmValue\" value."));
		return false;
	}

	if (!jsonObject[F("fanMinTemperature")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"fanMinTemperature\" field in configuration must be of type \"uint8\"."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"fanMinTemperature\" field in configuration must be of type \"uint8\"."));
		return false;
	}

	if (!TL::SystemConfigurationEndpoint::isInRange(jsonObject[F("fanMinTemperature")].as<uint8_t>(), 45, 70))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"fanMinTemperature\" field is invalid. It should be between 45 and 70."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"fanMinTemperature\" field is invalid. It should be between 45 and 70."));
		return false;
	}

	if (!jsonObject[F("fanMaxTemperature")].is<uint8_t>())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"fanMaxTemperature\" field in configuration must be of type \"uint8\"."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"fanMaxTemperature\" field in configuration must be of type \"uint8\"."));
		return false;
	}

	if (!TL::SystemConfigurationEndpoint::isInRange(jsonObject[F("fanMaxTemperature")].as<uint8_t>(), 60, 90))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"fanMaxTemperature\" field is invalid. It should be between 60 and 90."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"fanMaxTemperature\" field is invalid. It should be between 60 and 90."));
		return false;
	}

	if (!TL::SystemConfigurationEndpoint::validateMinMax(jsonObject[F("fanMinTemperature")].as<uint8_t>(), jsonObject[F("fanMaxTemperature")].as<uint8_t>()))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The \"fanMinTemperature\" must be smaller than the \"fanMaxTemperature\" value."));
		TL::SystemConfigurationEndpoint::sendSimpleResponse(400, (String)F("The \"fanMinTemperature\" must be smaller than the \"fanMaxTemperature\" value."));
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
bool TL::SystemConfigurationEndpoint::isInRange(const long value, const long min, const long max)
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
bool TL::SystemConfigurationEndpoint::validateMinMax(const long min, const long max)
{
	return min < max;
}
