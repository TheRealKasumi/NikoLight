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
TesLight::Configuration *TesLight::SystemConfigurationEndpoint::configuration = nullptr;
std::function<bool()> TesLight::SystemConfigurationEndpoint::configChangedCallback = nullptr;

/**
 * @brief Add all request handler for this {@link TesLight::RestEndpoint} to the {@link TesLight::WebServerManager}.
 */
void TesLight::SystemConfigurationEndpoint::begin(TesLight::Configuration *_configuration, std::function<bool()> _configChangedCallback)
{
	TesLight::SystemConfigurationEndpoint::configuration = _configuration;
	TesLight::SystemConfigurationEndpoint::configChangedCallback = _configChangedCallback;
	webServerManager->addRequestHandler((getBaseUri() + F("config/system")).c_str(), http_method::HTTP_GET, TesLight::SystemConfigurationEndpoint::getSystemConfig);
	webServerManager->addRequestHandler((getBaseUri() + F("config/system")).c_str(), http_method::HTTP_POST, TesLight::SystemConfigurationEndpoint::postSystemConfig);
}

/**
 * @brief Return the system configuration to the client as binary data.
 */
void TesLight::SystemConfigurationEndpoint::getSystemConfig()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the system configuration."));
	TesLight::InMemoryBinaryFile binary(14);
	binary.writeByte((uint8_t)TesLight::SystemConfigurationEndpoint::configuration->getSystemConfig().logLevel);
	binary.writeByte((uint8_t)TesLight::SystemConfigurationEndpoint::configuration->getSystemConfig().lightSensorMode);
	binary.writeByte(TesLight::SystemConfigurationEndpoint::configuration->getSystemConfig().lightSensorThreshold);
	binary.writeByte(TesLight::SystemConfigurationEndpoint::configuration->getSystemConfig().lightSensorMinAmbientBrightness);
	binary.writeByte(TesLight::SystemConfigurationEndpoint::configuration->getSystemConfig().lightSensorMaxAmbientBrightness);
	binary.writeByte(TesLight::SystemConfigurationEndpoint::configuration->getSystemConfig().lightSensorMinLedBrightness);
	binary.writeByte(TesLight::SystemConfigurationEndpoint::configuration->getSystemConfig().lightSensorMaxLedBrightness);
	binary.writeByte(TesLight::SystemConfigurationEndpoint::configuration->getSystemConfig().regulatorPowerLimit);
	binary.writeByte(TesLight::SystemConfigurationEndpoint::configuration->getSystemConfig().regulatorHighTemperature);
	binary.writeByte(TesLight::SystemConfigurationEndpoint::configuration->getSystemConfig().regulatorCutoffTemperature);
	binary.writeByte(TesLight::SystemConfigurationEndpoint::configuration->getSystemConfig().fanMinPwmValue);
	binary.writeByte(TesLight::SystemConfigurationEndpoint::configuration->getSystemConfig().fanMaxPwmValue);
	binary.writeByte(TesLight::SystemConfigurationEndpoint::configuration->getSystemConfig().fanMinTemperature);
	binary.writeByte(TesLight::SystemConfigurationEndpoint::configuration->getSystemConfig().fanMaxTemperature);

	String encoded = TesLight::Base64Util::encode(binary.getData(), binary.getBytesWritten());
	if (encoded == F("BASE64_ERROR"))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to encode response."));
		webServer->send(500, F("application/octet-stream"), F("Failed to encode response."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	webServer->send(200, "application/octet-stream", encoded);
}

/**
 * @brief Receive the system configuration sent by the client.
 */
void TesLight::SystemConfigurationEndpoint::postSystemConfig()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the system configuration."));

	if (!webServer->hasArg(F("data")) || webServer->arg(F("data")).length() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a x-www-form-urlencoded body parameter \"data\" with the base64 encoded system data."));
		webServer->send(400, F("text/plain"), F("There must be a body parameter \"data\" with the base64 encoded system data."));
		return;
	}

	const String encoded = webServer->arg(F("data"));
	size_t length;
	uint8_t *decoded = TesLight::Base64Util::decode(encoded, length);
	if (decoded == nullptr)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to decode request."));
		webServer->send(500, F("text/plain"), F("Failed to decode request."));
		return;
	}

	if (length != 14)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Length of decoded data is invalid."));
		delete[] decoded;
		webServer->send(400, F("text/plain"), F("The length of the decoded data must be exactly 14 bytes."));
		return;
	}

	TesLight::InMemoryBinaryFile binary(length);
	binary.loadFrom(decoded, length);
	delete[] decoded;

	TesLight::Configuration::SystemConfig config;
	config.logLevel = binary.readByte();
	config.lightSensorMode = binary.readByte();
	config.lightSensorThreshold = binary.readByte();
	config.lightSensorMinAmbientBrightness = binary.readByte();
	config.lightSensorMaxAmbientBrightness = binary.readByte();
	config.lightSensorMinLedBrightness = binary.readByte();
	config.lightSensorMaxLedBrightness = binary.readByte();
	config.regulatorPowerLimit = binary.readByte();
	config.regulatorHighTemperature = binary.readByte();
	config.regulatorCutoffTemperature = binary.readByte();
	config.fanMinPwmValue = binary.readByte();
	config.fanMaxPwmValue = binary.readByte();
	config.fanMinTemperature = binary.readByte();
	config.fanMaxTemperature = binary.readByte();

	if (!TesLight::SystemConfigurationEndpoint::validateLogLevel((uint8_t)config.logLevel))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The received log level is invalid."));
		webServer->send(400, F("text/plain"), F("The received log level is invalid."));
		return;
	}
	if (!TesLight::SystemConfigurationEndpoint::validateLightSensorMode((uint8_t)config.lightSensorMode))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The received light sensor mode is invalid."));
		webServer->send(400, F("text/plain"), F("The received light sensor mode is invalid."));
		return;
	}
	if (!TesLight::SystemConfigurationEndpoint::validateMinMax(config.lightSensorMinAmbientBrightness, config.lightSensorMaxAmbientBrightness))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The minimum ambient brightness must be smaller than the max value."));
		webServer->send(400, F("text/plain"), F("The minimum ambient brightness must be smaller than the max value."));
		return;
	}
	if (!TesLight::SystemConfigurationEndpoint::validateMinMax(config.lightSensorMinLedBrightness, config.lightSensorMaxLedBrightness))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The minimum LED brightness for automatic adjustment must be smaller than the max value."));
		webServer->send(400, F("text/plain"), F("The minimum LED brightness for automatic adjustment must be smaller than the max value."));
		return;
	}
	if (!TesLight::SystemConfigurationEndpoint::validateRegulatorPowerLimit(config.regulatorPowerLimit))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The regulator power limit must be between 1 and 30W."));
		webServer->send(400, F("text/plain"), F("The regulator power limit must be between 1 and 30W."));
		return;
	}
	if (!TesLight::SystemConfigurationEndpoint::validateMinMax(config.regulatorHighTemperature, config.regulatorCutoffTemperature))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The regulator high temperature must be lower than the cutoff temperature."));
		webServer->send(400, F("text/plain"), F("The regulator high temperature must be lower than the cutoff temperature."));
		return;
	}
	if (!TesLight::SystemConfigurationEndpoint::validateRegulatorHighTemperature(config.regulatorHighTemperature))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The regulator high temperature must be between 60°C and 90°C."));
		webServer->send(400, F("text/plain"), F("The regulator high temperature must be between 60°C and 90°C."));
		return;
	}
	if (!TesLight::SystemConfigurationEndpoint::validateRegulatorCutoffTemperature(config.regulatorCutoffTemperature))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The regulator cutoff temperature must be between 60°C and 100°C."));
		webServer->send(400, F("text/plain"), F("The regulator cutoff temperature must be between 60°C and 100°C."));
		return;
	}
	if (!TesLight::SystemConfigurationEndpoint::validateMinMax(config.fanMinPwmValue, config.fanMaxPwmValue))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The fan min pwm value must be smaller than the max value."));
		webServer->send(400, F("text/plain"), F("The fan min pwm value must be smaller than the max value."));
		return;
	}
	if (!TesLight::SystemConfigurationEndpoint::validateMinMax(config.fanMinTemperature, config.fanMaxTemperature))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The fan min temperature value must be smaller than the max value."));
		webServer->send(400, F("text/plain"), F("The fan min temperature value must be smaller than the max value."));
		return;
	}
	if (!TesLight::SystemConfigurationEndpoint::validateMinFanTemperature(config.fanMinTemperature))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The fan starting temperature must be between 40°C and 70°C."));
		webServer->send(400, F("text/plain"), F("The fan starting temperature must be between 40°C and 70°C."));
		return;
	}
	if (!TesLight::SystemConfigurationEndpoint::validateMaxFanTemperature(config.fanMaxTemperature))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The fan max temperature must be between 50°C and 90°C."));
		webServer->send(400, F("text/plain"), F("The fan max temperature must be between 50°C and 90°C."));
		return;
	}

	TesLight::SystemConfigurationEndpoint::configuration->setSystemConfig(config);
	if (TesLight::SystemConfigurationEndpoint::configuration->save())
	{
		if (TesLight::SystemConfigurationEndpoint::configChangedCallback)
		{
			if (!TesLight::SystemConfigurationEndpoint::configChangedCallback())
			{
				TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The callback function returned with an error."));
				webServer->send(500, F("text/plain"), F("Failed to call the callback function."));
				return;
			}
		}
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save system configuration."));
		webServer->send(500, F("text/plain"), F("Failed to save system configuration."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	webServer->send(200);
}

/**
 * @brief Validate if the min is smaller than the max value.
 * @param min received min value
 * @param max received max value
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::SystemConfigurationEndpoint::validateMinMax(const uint16_t min, const uint16_t max)
{
	return min < max;
}

/**
 * @brief Validate if the log level is valid.
 * @param value received log level
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::SystemConfigurationEndpoint::validateLogLevel(const uint8_t value)
{
	return value <= 3;
}

/**
 * @brief Validate if the light sensor mode is valid.
 * @param value received light sensor mode
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::SystemConfigurationEndpoint::validateLightSensorMode(const uint8_t value)
{
	return value <= 5;
}

/**
 * @brief Validate the maximum regulator power.
 * @param value value to validate
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::SystemConfigurationEndpoint::validateRegulatorPowerLimit(const uint8_t value)
{
	return value > 0 && value <= 30;
}

/**
 * @brief Validate the regulator high temperature.
 * @param value value to validate
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::SystemConfigurationEndpoint::validateRegulatorHighTemperature(const uint8_t value)
{
	return value >= 60 && value <= 90;
}

/**
 * @brief Validate the regulator cutoff temperature.
 * @param value value to validate
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::SystemConfigurationEndpoint::validateRegulatorCutoffTemperature(const uint8_t value)
{
	return value >= 60 && value <= 100;
}

/**
 * @brief Validate the minimum temperature where the fan starts.
 * @param value value to validate
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::SystemConfigurationEndpoint::validateMinFanTemperature(const uint8_t value)
{
	return value >= 40 && value <= 70;
}

/**
 * @brief Validate the maximum temperature where the fun runs at full speed.
 * @param value value to validate
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::SystemConfigurationEndpoint::validateMaxFanTemperature(const uint8_t value)
{
	return value >= 50 && value <= 90;
}
