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
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("SystemConfigurationEndpoint.cpp:getSystemConfig"), F("Received request to get the system configuration..."));

	TesLight::InMemoryBinaryFile binary(8);
	binary.writeByte((uint8_t)configuration->getSystemConfig().logLevel);
	binary.writeByte((uint8_t)configuration->getSystemConfig().lightSensorMode);
	binary.writeWord(configuration->getSystemConfig().lightSensorThreshold);
	binary.writeWord(configuration->getSystemConfig().lightSensorMinValue);
	binary.writeWord(configuration->getSystemConfig().lightSensorMaxValue);

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("SystemConfigurationEndpoint.cpp:getSystemConfig"), F("Preparing base64 response..."));

	size_t base64Length;
	unsigned char *base64 = base64_encode((unsigned char *)binary.getData(), binary.getBytesWritten(), &base64Length);

	String base64String;
	base64String.reserve(base64Length);
	for (size_t i = 0; i < base64Length; i++)
	{
		base64String += (char)base64[i];
	}
	free(base64);

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("SystemConfigurationEndpoint.cpp:getSystemConfig"), F("Sending the response."));

	request->send(200, "application/octet-stream", base64String);
}

/**
 * @brief Receive the system configuration sent by the client.
 * @param request instance of {@link AsyncWebServerRequest}
 */
void TesLight::SystemConfigurationEndpoint::postSystemConfig(AsyncWebServerRequest *request)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("SystemConfigurationEndpoint.cpp::postSystemConfig"), F("Received request to update the system configuration..."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("SystemConfigurationEndpoint.cpp::postSystemConfig"), F("Checking content-length..."));
	if (request->contentLength() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, F("SystemConfigurationEndpoint.cpp::postSystemConfig"), F("Content-length must not be null. The request could not be processed because it's empty."));
		request->send(400, F("text/plain"), F("A request body must be provided and the content-length must not be null."));
		return;
	}
	else if (request->contentType() != F("application/x-www-form-urlencoded"))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, F("SystemConfigurationEndpoint.cpp::postSystemConfigRequest"), F("The content type must be \"application/x-www-form-urlencoded\"."));
		request->send(400, F("text/plain"), F("The content type must be \"application/x-www-form-urlencoded\"."));
		return;
	}
	else if (request->arg("data").length() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, F("SystemConfigurationEndpoint.cpp::postSystemConfig"), F("There must be a body parameter \"data\" with the base64 encoded system data."));
		request->send(400, F("text/plain"), F("There must be a body parameter \"data\" with the base64 encoded system data."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("SystemConfigurationEndpoint.cpp:postSystemConfig"), F("Decoding base64 request..."));

	const String base64String = request->arg("data");
	const char *base64 = base64String.c_str();

	size_t dataLength;
	unsigned char *data = base64_decode((unsigned char *)base64, base64String.length(), &dataLength);

	TesLight::InMemoryBinaryFile binary(dataLength);
	binary.loadFrom(data, dataLength);
	free(data);

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("SystemConfigurationEndpoint.cpp:postSystemConfig"), F("Parsing new system configuration..."));

	TesLight::Configuration::SystemConfig config;
	config.logLevel = (TesLight::Logger::LogLevel)binary.readByte();
	config.lightSensorMode = (TesLight::LightSensor::LightSensorMode)binary.readByte();
	config.lightSensorThreshold = binary.readWord();
	config.lightSensorMinValue = binary.readWord();
	config.lightSensorMaxValue = binary.readWord();

	if (!validateLogLevel((uint8_t)config.logLevel))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, F("SystemConfigurationEndpoint.cpp::postSystemConfig"), F("The received log level is invalid."));
		request->send(400, F("text/plain"), F("The received log level is invalid."));
		return;
	}
	if (!validateLightSensorMode((uint8_t)config.lightSensorMode))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, F("SystemConfigurationEndpoint.cpp::postSystemConfig"), F("The received light sensor mode is invalid."));
		request->send(400, F("text/plain"), F("The received light sensor mode is invalid."));
		return;
	}
	if (!validateMinMax(config.lightSensorMinValue, config.lightSensorMaxValue))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, F("SystemConfigurationEndpoint.cpp::postSystemConfig"), F("The light sensor min value must be smaller than the max value."));
		request->send(400, F("text/plain"), F("The light sensor min value must be smaller than the max value."));
		return;
	}

	configuration->setSystemConfig(config);
	if (configuration->save())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("SystemConfigurationEndpoint.cpp:postSystemConfig"), F("System configuration saved. Executing callback function..."));
		if (configChangedCallback)
		{
			configChangedCallback();
		}
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("SystemConfigurationEndpoint.cpp:postSystemConfig"), F("Failed to save system configuration..."));
		request->send(500, F("text/plain"), (String)F("Failed to save system configuration."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("SystemConfigurationEndpoint.cpp::postSystemConfig"), F("System configuration saved. Sending the response."));
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
bool TesLight::SystemConfigurationEndpoint::validateMinMax(const uint8_t min, const uint8_t max)
{
	return max > min;
}