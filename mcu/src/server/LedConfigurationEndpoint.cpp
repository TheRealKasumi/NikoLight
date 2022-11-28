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
	TesLight::InMemoryBinaryFile binary(232);
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		binary.writeByte(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).ledPin);
		binary.writeWord(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).ledCount);
		binary.writeByte(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).type);
		binary.writeByte(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).speed);
		binary.writeWord(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).offset);
		binary.writeByte(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).brightness);
		binary.writeByte(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).reverse);
		binary.writeByte(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).fadeSpeed);
		for (uint8_t j = 0; j < ANIMATOR_NUM_CUSTOM_FIELDS; j++)
		{
			binary.writeByte(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).customField[j]);
		}
		binary.writeByte(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).ledVoltage);
		binary.writeByte(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).ledChannelCurrent[0]);
		binary.writeByte(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).ledChannelCurrent[1]);
		binary.writeByte(TesLight::LedConfigurationEndpoint::configuration->getLedConfig(i).ledChannelCurrent[2]);
	}

	String encoded = TesLight::Base64Util::encode(binary.getData(), binary.getBytesWritten());
	if (encoded == F("BASE64_ERROR"))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to encode response."));
		webServer->send(500, F("application/octet-stream"), F("Failed to encode response."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	webServer->send(200, F("application/octet-stream"), encoded);
}

/**
 * @brief Receive the LED configuration sent by the client.
 */
void TesLight::LedConfigurationEndpoint::postLedConfig()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the LED configuration."));

	if (!webServer->hasArg(F("data")) || webServer->arg(F("data")).length() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a x-www-form-urlencoded body parameter \"data\" with the base64 encoded LED data."));
		webServer->send(400, F("text/plain"), F("There must be a body parameter \"data\" with the base64 encoded LED data."));
		return;
	}

	const String encoded = webServer->arg(F("data"));
	size_t length;
	uint8_t *decoded = TesLight::Base64Util::decode(encoded, length);
	if (decoded == nullptr)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to decode request."));
		webServer->send(500, F("application/octet-stream"), F("Failed to decode request."));
		return;
	}

	if (length != 232)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Length of decoded data is invalid."));
		delete[] decoded;
		webServer->send(400, F("text/plain"), F("The length of the decoded data must be exactly 232 bytes."));
		return;
	}

	TesLight::InMemoryBinaryFile binary(length);
	binary.loadFrom(decoded, length);
	delete[] decoded;

	TesLight::Configuration::LedConfig config[LED_NUM_ZONES];
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		config[i].ledPin = binary.readByte();
		config[i].ledCount = binary.readWord();
		config[i].type = binary.readByte();
		config[i].speed = binary.readByte();
		config[i].offset = binary.readWord();
		config[i].brightness = binary.readByte();
		config[i].reverse = binary.readByte();
		config[i].fadeSpeed = binary.readByte();
		for (uint8_t j = 0; j < ANIMATOR_NUM_CUSTOM_FIELDS; j++)
		{
			config[i].customField[j] = binary.readByte();
		}
		config[i].ledVoltage = binary.readByte();
		config[i].ledChannelCurrent[0] = binary.readByte();
		config[i].ledChannelCurrent[1] = binary.readByte();
		config[i].ledChannelCurrent[2] = binary.readByte();

		if (!validateLedPin(config[i].ledPin) || !validateLedCount(config[i].ledCount) || !validateAnimatorType(config[i].type))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("The LED configuration at index ") + String(i) + F(" is invalid."));
			webServer->send(400, F("text/plain"), (String)F("The LED configuration at index ") + String(i) + F(" is invalid."));
			return;
		}
	}

	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		TesLight::LedConfigurationEndpoint::configuration->setLedConfig(config[i], i);
	}

	if (TesLight::LedConfigurationEndpoint::configuration->save())
	{
		if (TesLight::LedConfigurationEndpoint::configChangedCallback)
		{
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

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("LED configuration saved. Sending the response."));
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
	const uint8_t ledOutputPins[LED_NUM_ZONES] = LED_DEFAULT_OUTPUT_PINS;
	for (uint8_t i = 0; i < LED_NUM_ZONES; i++)
	{
		if (ledOutputPins[i] == ledPin)
		{
			return true;
		}
	}

	return false;
}

/**
 * @brief Validate if the LED count is valid.
 * @param ledCount number of LEDs
 * @return true when value
 * @return false when invalid
 */
bool TesLight::LedConfigurationEndpoint::validateLedCount(const int ledCount)
{
	return ledCount > 1 && ledCount < 255;
}

/**
 * @brief Validate if the animator type is valid.
 * @param animatorType type of the animator
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::LedConfigurationEndpoint::validateAnimatorType(const int animatorType)
{
	return (animatorType >= 0 && animatorType <= 17) || animatorType == 255;
}
