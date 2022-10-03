/**
 * @file LedConfigurationEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to configure the LED settings.
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
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Register LED Configuration Endpoints."));
	configuration = _configuration;
	configChangedCallback = _configChangedCallback;
	getWebServer()->addRequestHandler((getBaseUri() + F("config/led")).c_str(), http_method::HTTP_GET, TesLight::LedConfigurationEndpoint::getLedConfig);
	getWebServer()->addRequestHandler((getBaseUri() + F("config/led")).c_str(), http_method::HTTP_POST, TesLight::LedConfigurationEndpoint::postLedConfig);
}

/**
 * @brief Return the LED configuration to the client as binary data.
 * @param request instance of {@link AsyncWebServerRequest}
 */
void TesLight::LedConfigurationEndpoint::getLedConfig(AsyncWebServerRequest *request)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the LED configuration."));
	TesLight::InMemoryBinaryFile binary(120);
	for (uint8_t i = 0; i < NUM_LED_STRIPS; i++)
	{
		binary.writeByte(configuration->getLedConfig(i).ledPin);
		binary.writeWord(configuration->getLedConfig(i).ledCount);
		binary.writeByte(configuration->getLedConfig(i).type);
		binary.writeByte(configuration->getLedConfig(i).speed);
		binary.writeWord(configuration->getLedConfig(i).offset);
		binary.writeByte(configuration->getLedConfig(i).brightness);
		binary.writeByte(configuration->getLedConfig(i).reverse);
		binary.writeByte(configuration->getLedConfig(i).fadeSpeed);
		for (uint8_t j = 0; j < NUM_ANIMATOR_CUSTOM_FIELDS; j++)
		{
			binary.writeByte(configuration->getLedConfig(i).customField[j]);
		}
	}

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
	request->send(200, F("application/octet-stream"), encoded);
}

/**
 * @brief Receive the LED configuration sent by the client.
 * @param request instance of {@link AsyncWebServerRequest}
 */
void TesLight::LedConfigurationEndpoint::postLedConfig(AsyncWebServerRequest *request)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the LED configuration."));
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
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a body parameter \"data\" with the base64 encoded LED data."));
		request->send(400, F("text/plain"), F("There must be a body parameter \"data\" with the base64 encoded LED data."));
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

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Parsing new LED configuration."));
	TesLight::Configuration::LedConfig config[6];
	for (uint8_t i = 0; i < NUM_LED_STRIPS; i++)
	{
		config[i].ledPin = binary.readByte();
		config[i].ledCount = binary.readWord();
		config[i].type = binary.readByte();
		config[i].speed = binary.readByte();
		config[i].offset = binary.readWord();
		config[i].brightness = binary.readByte();
		config[i].reverse = binary.readByte();
		config[i].fadeSpeed = binary.readByte();
		for (uint8_t j = 0; j < NUM_ANIMATOR_CUSTOM_FIELDS; j++)
		{
			config[i].customField[j] = binary.readByte();
		}

		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Validating LED configuration with index ") + String(i) + ".");
		if (!validateLedPin(config[i].ledPin) || !validateLedCount(config[i].ledCount) || !validateAnimatorType(config[i].type))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The LED configuration at index ") + String(i) + F(" is invalid."));
			request->send(400, F("text/plain"), (String)F("The LED configuration at index ") + String(i) + F(" is invalid."));
			return;
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("LED configuration is valid. Saving LED configuration."));
	for (uint8_t i = 0; i < NUM_LED_STRIPS; i++)
	{
		configuration->setLedConfig(config[i], i);
	}

	if (configuration->save())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("LED configuration saved. Executing callback function."));
		if (configChangedCallback)
		{
			configChangedCallback();
		}
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save LED configuration."));
		request->send(500, F("text/plain"), (String)F("Failed to save LED configuration."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("LED configuration saved. Sending the response."));
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
	return (animatorType >= 0 && animatorType <= 4) || animatorType == 255;
}
