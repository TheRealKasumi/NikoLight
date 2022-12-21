/**
 * @file ResetEndpoint.h
 * @author TheRealKasumi
 * @brief Contains a REST endpoint to remotely reset the controller in case something is going wrong.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "server/ResetEndpoint.h"

FS *TesLight::ResetEndpoint::fileSystem = nullptr;

/**
 * @brief Add all request handler for this {@link TesLight::RestEndpoint} to the {@link TesLight::WebServerManager}.
 */
void TesLight::ResetEndpoint::begin(FS *_fileSystem)
{
	ResetEndpoint::fileSystem = _fileSystem;
	webServerManager->addRequestHandler((getBaseUri() + F("reset/soft")).c_str(), http_method::HTTP_POST, TesLight::ResetEndpoint::handleSoftReset);
	webServerManager->addRequestHandler((getBaseUri() + F("reset/hard")).c_str(), http_method::HTTP_POST, TesLight::ResetEndpoint::handleHardReset);
}

/**
 * @brief Handler function for the soft reset.
 */
void TesLight::ResetEndpoint::handleSoftReset()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to execute a soft reset."));
	webServer->send(200, F("application/text"), F("Controller will reboot."));

	delay(250);
	TesLight::Updater::reboot(F("Soft Reset"));
}

/**
 * @brief Handler function for the hard reset. This also deletes the configuration to start with defaults.
 */
void TesLight::ResetEndpoint::handleHardReset()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to execute a hard reset."));
	webServer->send(200, F("application/text"), F("The configuration will be reset to defaults. The controller will then reboot. Make sure to reconnect using the default SSID and password."));

	if (!TesLight::ResetEndpoint::fileSystem->remove(CONFIGURATION_FILE_NAME))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to remove configuration. This might be normal if it was not saved before."));
	}
	else
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Configuration was reset."));
	}

	delay(250);
	TesLight::Updater::reboot(F("Hard Reset"));
}