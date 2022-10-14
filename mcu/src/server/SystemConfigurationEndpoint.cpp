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
std::function<void()> TesLight::SystemConfigurationEndpoint::configChangedCallback = nullptr;

/**
 * @brief Add all request handler for this {@link TesLight::RestEndpoint} to the {@link TesLight::WebServer}.
 */
void TesLight::SystemConfigurationEndpoint::begin(TesLight::Configuration *_configuration, std::function<void()> _configChangedCallback)
{
	configuration = _configuration;
	configChangedCallback = _configChangedCallback;
	getWebServer()->addRequestHandler((getBaseUri() + F("config/system")).c_str(), http_method::HTTP_GET, TesLight::SystemConfigurationEndpoint::getSystemConfig);
	getWebServer()->addRequestHandler((getBaseUri() + F("config/system")).c_str(), http_method::HTTP_POST, TesLight::SystemConfigurationEndpoint::postSystemConfig);
}

/**
 * @brief Return the system configuration to the client as binary data.
 * @param request instance of {@link AsyncWebServerRequest}
 */
void TesLight::SystemConfigurationEndpoint::getSystemConfig(AsyncWebServerRequest *request)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the system configuration."));
	TesLight::InMemoryBinaryFile binary(13);
	binary.writeByte((uint8_t)configuration->getSystemConfig().logLevel);
	binary.writeByte((uint8_t)configuration->getSystemConfig().lightSensorMode);
	binary.writeWord(configuration->getSystemConfig().lightSensorThreshold);
	binary.writeWord(configuration->getSystemConfig().lightSensorMinValue);
	binary.writeWord(configuration->getSystemConfig().lightSensorMaxValue);
	binary.writeByte(configuration->getSystemConfig().systemPowerLimit);
	binary.writeByte(configuration->getSystemConfig().ledVoltage);
	binary.writeByte(configuration->getSystemConfig().ledChannelCurrent[0]);
	binary.writeByte(configuration->getSystemConfig().ledChannelCurrent[1]);
	binary.writeByte(configuration->getSystemConfig().ledChannelCurrent[2]);

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Preparing base64 response."));
	String encoded = TesLight::Base64Util::encode(binary.getData(), binary.getBytesWritten());
	if (encoded == F("BASE64_ERROR"))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to encode response."));
		request->send(500, F("application/octet-stream"), F("Failed to encode response."));
		return;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Base64 response prepared."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Sending the response."));
	request->send(200, "application/octet-stream", encoded);
}

/**
 * @brief Receive the system configuration sent by the client.
 * @param request instance of {@link AsyncWebServerRequest}
 */
void TesLight::SystemConfigurationEndpoint::postSystemConfig(AsyncWebServerRequest *request)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the system configuration."));
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Checking content-length."));
	if (request->contentLength() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Content-length must not be null. The request could not be processed because it's empty."));
		request->send(400, F("text/plain"), F("A request body must be provided and the content-length must not be null."));
		return;
	}
	else if (request->contentType() != F("application/x-www-form-urlencoded"))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The content type must be \"application/x-www-form-urlencoded\"."));
		request->send(400, F("text/plain"), F("The content type must be \"application/x-www-form-urlencoded\"."));
		return;
	}
	else if (request->arg(F("data")).length() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a body parameter \"data\" with the base64 encoded system data."));
		request->send(400, F("text/plain"), F("There must be a body parameter \"data\" with the base64 encoded system data."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Decoding base64 request."));
	const String encoded = request->arg(F("data"));
	size_t length;
	uint8_t *decoded = TesLight::Base64Util::decode(encoded, length);
	if (decoded == nullptr)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to decode request."));
		request->send(500, F("application/octet-stream"), F("Failed to decode request."));
		return;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Request decoded."));

	TesLight::InMemoryBinaryFile binary(length);
	binary.loadFrom(decoded, length);
	delete[] decoded;

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Parsing new system configuration."));
	TesLight::Configuration::SystemConfig config;
	config.logLevel = (TesLight::Logger::LogLevel)binary.readByte();
	config.lightSensorMode = (TesLight::LightSensor::LightSensorMode)binary.readByte();
	config.lightSensorThreshold = binary.readWord();
	config.lightSensorMinValue = binary.readWord();
	config.lightSensorMaxValue = binary.readWord();
	config.systemPowerLimit = binary.readByte();
	config.ledVoltage = binary.readByte();
	config.ledChannelCurrent[0] = binary.readByte();
	config.ledChannelCurrent[1] = binary.readByte();
	config.ledChannelCurrent[2] = binary.readByte();

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Validating system configuration."));
	if (!validateLogLevel((uint8_t)config.logLevel))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The received log level is invalid."));
		request->send(400, F("text/plain"), F("The received log level is invalid."));
		return;
	}
	if (!validateLightSensorMode((uint8_t)config.lightSensorMode))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The received light sensor mode is invalid."));
		request->send(400, F("text/plain"), F("The received light sensor mode is invalid."));
		return;
	}
	if (!validateMinMax(config.lightSensorMinValue, config.lightSensorMaxValue))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The light sensor min value must be smaller than the max value."));
		request->send(400, F("text/plain"), F("The light sensor min value must be smaller than the max value."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Saving system configuration."));
	configuration->setSystemConfig(config);
	if (configuration->save())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("System configuration saved. Executing callback function."));
		if (configChangedCallback)
		{
			configChangedCallback();
		}
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save system configuration."));
		request->send(500, F("text/plain"), (String)F("Failed to save system configuration."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("System configuration saved. Sending the response."));
	request->send(202);
}

/**
 * @brief Validate if the log level is valid.
 * @param logLevel received log level
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::SystemConfigurationEndpoint::validateLogLevel(const uint8_t logLevel)
{
	return logLevel <= 3;
}

/**
 * @brief Validate if the light sensor mode is valid.
 * @param lightSensorMode received light sensor mode
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::SystemConfigurationEndpoint::validateLightSensorMode(const uint8_t lightSensorMode)
{
	return lightSensorMode <= 3;
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
