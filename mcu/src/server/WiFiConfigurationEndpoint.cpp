/**
 * @file WiFiConfigurationEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to configure the WiFi settings.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "server/WiFiConfigurationEndpoint.h"

// Initialize
TesLight::Configuration *TesLight::WiFiConfigurationEndpoint::configuration = nullptr;
std::function<void()> TesLight::WiFiConfigurationEndpoint::configChangedCallback = nullptr;

/**
 * @brief Add all request handler for this {@link TesLight::RestEndpoint} to the {@link TesLight::WebServer}.
 */
void TesLight::WiFiConfigurationEndpoint::begin(TesLight::Configuration *_configuration, std::function<void()> _configChangedCallback)
{
	configuration = _configuration;
	configChangedCallback = _configChangedCallback;
	getWebServer()->addRequestHandler((getBaseUri() + F("config/wifi")).c_str(), http_method::HTTP_GET, TesLight::WiFiConfigurationEndpoint::getWiFiConfig);
	getWebServer()->addRequestHandler((getBaseUri() + F("config/wifi")).c_str(), http_method::HTTP_POST, TesLight::WiFiConfigurationEndpoint::postWiFiConfig);
}

/**
 * @brief Return the WiFi configuration to the client.
 * @param request instance of {@link AsyncWebServerRequest}
 */
void TesLight::WiFiConfigurationEndpoint::getWiFiConfig(AsyncWebServerRequest *request)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the WiFi configuration."));
	TesLight::InMemoryBinaryFile binary(256);
	binary.writeString(configuration->getWiFiConfig().accessPointSsid);
	binary.writeString(configuration->getWiFiConfig().accessPointPassword);
	binary.writeByte(configuration->getWiFiConfig().accessPointChannel);
	binary.writeByte(configuration->getWiFiConfig().accessPointHidden);
	binary.writeByte(configuration->getWiFiConfig().accessPointMaxConnections);
	binary.writeString(configuration->getWiFiConfig().wifiSsid);
	binary.writeString(configuration->getWiFiConfig().wifiPassword);

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
	request->send(200, F("octet-stream"), encoded);
}

/**
 * @brief Receive the WiFi configuration sent by the client.
 * @param request instance of {@link AsyncWebServerRequest}
 */
void TesLight::WiFiConfigurationEndpoint::postWiFiConfig(AsyncWebServerRequest *request)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to update the WiFi configuration."));
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
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("There must be a body parameter \"data\" with the base64 encoded WiFi data."));
		request->send(400, F("text/plain"), F("There must be a body parameter \"data\" with the base64 encoded WiFi data."));
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

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Parsing new WiFi configuration."));
	TesLight::Configuration::WiFiConfig config;
	config.accessPointSsid = binary.readString();
	config.accessPointPassword = binary.readString();
	config.accessPointChannel = binary.readByte();
	config.accessPointHidden = binary.readByte();
	config.accessPointMaxConnections = binary.readByte();
	config.wifiSsid = binary.readString();
	config.wifiPassword = binary.readString();

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Validating WiFi configuration."));
	if (!validateWiFiSsid(config.accessPointSsid) || !validateWiFiPassword(config.accessPointPassword) || !validateWiFiChannel(config.accessPointChannel) ||
		!validateWiFiMaxConnections(config.accessPointMaxConnections))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The access point configuration is invalid."));
		request->send(400, F("text/plain"), (String)F("The access point configuration is invalid."));
		return;
	}

	if ((config.wifiSsid != "" && !validateWiFiSsid(config.wifiSsid)) || (config.wifiPassword != "" && !validateWiFiPassword(config.wifiPassword)))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The WiFi configuration is invalid."));
		request->send(400, F("text/plain"), (String)F("The WiFi configuration is invalid."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("WiFi configuration is valid. Saving WiFi configuration."));
	configuration->setWiFiConfig(config);
	if (configuration->save())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("WiFi configuration saved. Executing callback function."));
		if (configChangedCallback)
		{
			configChangedCallback();
		}
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to save WiFi configuration."));
		request->send(500, F("text/plain"), (String)F("Failed to save WiFi configuration."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("WiFi configuration saved. Sending the response."));
	request->send(202);
}

/**
 * @brief Verify if a WiFi ssid is valid.
 * @param ssid ssid to check
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::WiFiConfigurationEndpoint::validateWiFiSsid(const String ssid)
{
	return ssid.length() >= 2 && ssid.length() <= 32 && hasValidSsidChars(ssid);
}

/**
 * @brief Verify if a WiFi password is valid.
 * @param password password to check
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::WiFiConfigurationEndpoint::validateWiFiPassword(const String password)
{
	return password.length() >= 8 && password.length() <= 63 && hasValidPasswordChars(password);
}

/**
 * @brief Verify if a WiFi channel is valid.
 * @param channel channel number
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::WiFiConfigurationEndpoint::validateWiFiChannel(const int channel)
{
	return channel > 0 && channel < 13;
}

/**
 * @brief Verify if the connection number is valid.
 * @param maxConnections maximum number of WiFi connections
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::WiFiConfigurationEndpoint::validateWiFiMaxConnections(const int maxConnections)
{
	return maxConnections > 0 && maxConnections < 5;
}

/**
 * @brief Verify if the input only contains valid WiFi ssid characters.
 * @param input input {@link String}
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::WiFiConfigurationEndpoint::hasValidSsidChars(const String input)
{
	const String validChars = F("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-_ ");
	for (uint16_t i = 0; i < input.length(); i++)
	{
		if (validChars.indexOf(input.charAt(i)) == -1)
		{
			return false;
		}
	}

	return true;
}

/**
 * @brief Verify if the input only contains valid WiFi password characters.
 * @param input input {@link String}
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::WiFiConfigurationEndpoint::hasValidPasswordChars(const String input)
{
	const String validChars = F("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+-_!\"§$%&/()=?*# ");
	for (uint16_t i = 0; i < input.length(); i++)
	{
		if (validChars.indexOf(input.charAt(i)) == -1)
		{
			return false;
		}
	}

	return true;
}
