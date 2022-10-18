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
std::function<bool()> TesLight::LedConfigurationEndpoint::configChangedCallback = nullptr;

/**
 * @brief Add all request handler for this {@link TesLight::RestEndpoint} to the {@link TesLight::WebServerManager}.
 */
void TesLight::LedConfigurationEndpoint::begin(TesLight::Configuration *_configuration, std::function<bool()> _configChangedCallback)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Register LED Configuration Endpoints."));
	TesLight::LedConfigurationEndpoint::configuration = _configuration;
	TesLight::LedConfigurationEndpoint::configChangedCallback = _configChangedCallback;
	webServerManager->addRequestHandler((getBaseUri() + F("config/led")).c_str(), http_method::HTTP_GET, TesLight::LedConfigurationEndpoint::getLedConfig);
	webServerManager->addRequestHandler((getBaseUri() + F("config/led")).c_str(), http_method::HTTP_POST, TesLight::LedConfigurationEndpoint::postLedConfig);
}

/**
 * @brief Return the LED configuration to the client as binary data.
 */
void TesLight::LedConfigurationEndpoint::getLedConfig()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the LED configuration."));
	TesLight::InMemoryBinaryFile binary(120);
	for (uint8_t i = 0; i < NUM_LED_STRIPS; i++)
	{
		binary.writeByte(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).ledPin);
		binary.writeWord(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).ledCount);
		binary.writeByte(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).type);
		binary.writeByte(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).speed);
		binary.writeWord(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).offset);
		binary.writeByte(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).brightness);
		binary.writeByte(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).reverse);
		binary.writeByte(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).fadeSpeed);
		for (uint8_t j = 0; j < NUM_ANIMATOR_CUSTOM_FIELDS; j++)
		{
			binary.writeByte(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).customField[j]);
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Preparing base64 response."));
	String encoded = TesLight::Base64Util::encode(binary.getData(), binary.getBytesWritten());
	if (encoded == F("BASE64_ERROR"))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to encode response."));
		webServer->send(500, F("application/octet-stream"), F("Failed to encode response."));
		return;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Base64 response prepared."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Sending the response."));
	webServer->send(200, F("application/octet-stream"), encoded);
}

/**
 * @brief Receive the LED configuration sent by the client.
 */
void TesLight::LedConfigurationEndpoint::postLedConfig()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the LED configuration."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Checking request."));
	if (!webServer->hasArg(F("data")) || webServer->arg(F("data")).length() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a x-www-form-urlencoded body parameter \"data\" with the base64 encoded LED data."));
		webServer->send(400, F("text/plain"), F("There must be a body parameter \"data\" with the base64 encoded LED data."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Decoding base64 request."));
	const String encoded = webServer->arg(F("data"));
	size_t length;
	uint8_t *decoded = TesLight::Base64Util::decode(encoded, length);
	if (decoded == nullptr)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to decode request."));
		webServer->send(500, F("application/octet-stream"), F("Failed to decode request."));
		return;
	}
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Request decoded."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Checking length of the decoded data."));
	if (length != 120)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Length of decoded data is invalid."));
		delete[] decoded;
		webServer->send(400, F("text/plain"), F("The length of the decoded data must be exactly 120 bytes."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Decoded data is ok, loading into binary buffer."));
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
			webServer->send(400, F("text/plain"), (String)F("The LED configuration at index ") + String(i) + F(" is invalid."));
			return;
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("LED configuration is valid. Saving LED configuration."));
	for (uint8_t i = 0; i < NUM_LED_STRIPS; i++)
	{
		TesLight::LedConfigurationEndpoint::configuration->setLedConfig(config[i], i);
	}

	if (TesLight::LedConfigurationEndpoint::configuration->save())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("LED configuration saved."));
		if (TesLight::LedConfigurationEndpoint::configChangedCallback)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Calling callback function."));
			if (!TesLight::LedConfigurationEndpoint::configChangedCallback())
			{
				TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The callback function returned with an error."));
				webServer->send(500, F("text/plain"), F("Failed to call the callback function."));
				return;
			}
		}
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save LED configuration."));
		webServer->send(500, F("text/plain"), F("Failed to save LED configuration."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("LED configuration saved. Sending the response."));
	webServer->send(200);
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
