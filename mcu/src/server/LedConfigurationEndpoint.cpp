/**
 * @file LedConfigurationEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to configure the LED settings.
 * @version 0.0.1
 * @date 2022-07-07
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "server/LedConfigurationEndpoint.h"

// Initialize
TesLight::Configuration *TesLight::LedConfigurationEndpoint::configuration = nullptr;
std::function<void()> TesLight::LedConfigurationEndpoint::configChangedCallback = nullptr;

/**
 * @brief Add all request handler for this {@link TesLight::RestEndpoint} to the {@link TesLight::WebServer}.
 */
void TesLight::LedConfigurationEndpoint::begin(TesLight::Configuration *_configuration, std::function<void()> _configChangedCallback)
{
	configuration = _configuration;
	configChangedCallback = _configChangedCallback;
	getWebServer()->addRequestHandler((getBaseUri() + F("led")).c_str(), http_method::HTTP_GET, TesLight::LedConfigurationEndpoint::getLedConfig);
	getWebServer()->addRequestHandler((getBaseUri() + F("led")).c_str(), http_method::HTTP_POST, TesLight::LedConfigurationEndpoint::postLedConfig);
}

/**
 * @brief Return the LED configuration to the client as binary data.
 * @param request instance of {@link AsyncWebServerRequest}
 */
void TesLight::LedConfigurationEndpoint::getLedConfig(AsyncWebServerRequest *request)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("LedConfigurationEndpoint.cpp:getLedConfig"), F("Received request to get the LED configuration..."));

	TesLight::InMemoryBinaryFile binary(256);
	for (uint8_t i = 0; i < NUM_LED_DRIVERS; i++)
	{
		binary.writeByte(configuration->getLedConfig(i).ledPin);
		binary.writeWord(configuration->getLedConfig(i).ledCount);
		binary.writeByte(configuration->getLedConfig(i).type);
		binary.writeByte(configuration->getLedConfig(i).speed);
		binary.writeWord(configuration->getLedConfig(i).offset);
		binary.writeByte(configuration->getLedConfig(i).brightness);
		binary.writeByte(configuration->getLedConfig(i).reverse);
		for (uint8_t j = 0; j < NUM_ANIMATOR_CUSTOM_FIELDS; j++)
		{
			binary.writeByte(configuration->getLedConfig(i).customField[j]);
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("LedConfigurationEndpoint.cpp:getLedConfig"), F("Preparing base64 response..."));

	size_t base64Length;
	unsigned char *base64 = base64_encode((unsigned char *)binary.getData(), binary.getBytesWritten(), &base64Length);

	String base64String;
	base64String.reserve(base64Length);
	for (size_t i = 0; i < base64Length; i++)
	{
		base64String += (char)base64[i];
	}
	free(base64);

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("LedConfigurationEndpoint.cpp:getLedConfig"), F("Sending the response."));

	request->send(200, "application/octet-stream", base64String);
}

/**
 * @brief Receive the LED configuration sent by the client.
 * @param request instance of {@link AsyncWebServerRequest}
 */
void TesLight::LedConfigurationEndpoint::postLedConfig(AsyncWebServerRequest *request)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, F("LedConfigurationEndpoint.cpp::postLedConfig"), F("Received request to update the LED configuration..."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("LedConfigurationEndpoint.cpp::postLedConfig"), F("Checking content-length..."));
	if (request->contentLength() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, F("LedConfigurationEndpoint.cpp::postLedConfig"), F("Content-length must not be null. The request could not be processed because it's empty."));
		request->send(400, F("text/plain"), F("A request body must be provided and the content-length must not be null."));
		return;
	}
	else if (request->contentType() != F("application/x-www-form-urlencoded"))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, F("LedConfigurationEndpoint.cpp::postLedConfigRequest"), F("The content type must be \"application/x-www-form-urlencoded\"."));
		request->send(400, F("text/plain"), (String)F("The content type must be \"application/x-www-form-urlencoded\"."));
		return;
	}
	else if (request->arg("data").length() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, F("LedConfigurationEndpoint.cpp::postLedConfig"), F("There must be a body parameter \"data\" with the base64 encoded LED data."));
		request->send(400, F("text/plain"), (String)F("There must be a body parameter \"data\" with the base64 encoded LED data."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("LedConfigurationEndpoint.cpp:postLedConfig"), F("Decoding base64 request..."));

	const String base64String = request->arg("data");
	const char *base64 = base64String.c_str();

	size_t dataLength;
	unsigned char *data = base64_decode((unsigned char *)base64, base64String.length(), &dataLength);

	TesLight::InMemoryBinaryFile binary(dataLength);
	binary.loadFrom(data, dataLength);
	free(data);

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("LedConfigurationEndpoint.cpp:postLedConfig"), F("Parsing new LED configuration..."));

	TesLight::Configuration::LedConfig config[6];
	for (uint8_t i = 0; i < NUM_LED_DRIVERS; i++)
	{
		config[i].ledPin = binary.readByte();
		config[i].ledCount = binary.readWord();
		config[i].type = binary.readByte();
		config[i].speed = binary.readByte();
		config[i].offset = binary.readWord();
		config[i].brightness = binary.readByte();
		config[i].reverse = binary.readByte();
		for (uint8_t j = 0; j < NUM_ANIMATOR_CUSTOM_FIELDS; j++)
		{
			config[i].customField[j] = binary.readByte();
		}

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("LedConfigurationEndpoint.cpp:postLedConfig"), (String)F("Validating LED configuration with index ") + String(i) + "...");

		if (!validateLedPin(config[i].ledPin) || !validateLedCount(config[i].ledCount) || !validateAnimatorType(config[i].type))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, F("LedConfigurationEndpoint.cpp::postLedConfig"), (String)F("The LED configuration at index ") + String(i) + F(" is invalid."));
			request->send(400, F("text/plain"), (String)F("The LED configuration at index ") + String(i) + F(" is invalid."));
			return;
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("LedConfigurationEndpoint.cpp:postLedConfig"), F("LED configuration is valid. Saving LED configuration..."));

	for (uint8_t i = 0; i < NUM_LED_DRIVERS; i++)
	{
		configuration->setLedConfig(config[i], i);
	}

	if (configuration->save())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("LedConfigurationEndpoint.cpp:postLedConfig"), F("LED configuration saved. Executing callback function..."));
		if (configChangedCallback)
		{
			configChangedCallback();
		}
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, F("LedConfigurationEndpoint.cpp:postLedConfig"), F("Failed to save LED configuration..."));
		request->send(500, F("text/plain"), (String)F("Failed to save LED configuration."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, F("LedConfigurationEndpoint.cpp::postLedConfig"), F("LED configuration saved. Sending the response."));

	request->send(202);
}

/**
 * @brief Verify if the pin is a valid led output pin.
 * @param ledPin pin number
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::LedConfigurationEndpoint::validateLedPin(int ledPin)
{
	return ledPin == 13 || ledPin == 14 || ledPin == 15 || ledPin == 16 || ledPin == 17 || ledPin == 21;
}

/**
 * @brief Validate if the LED count is valid.
 * @param ledCount number of LEDs
 * @return true when value
 * @return false when invalid
 */
bool TesLight::LedConfigurationEndpoint::validateLedCount(const int ledCount)
{
	return ledCount > 0 && ledCount < 255;
}

/**
 * @brief Validate if the animator type is valid.
 * @param animatorType type of the animator
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::LedConfigurationEndpoint::validateAnimatorType(const int animatorType)
{
	return animatorType >= 0 && animatorType <= 4;
}
