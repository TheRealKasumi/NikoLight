/**
 * @file UpdateEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to load update packages to the TesLight controller
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "server/UpdateEndpoint.h"

// Initialize
FS *TesLight::UpdateEndpoint::fileSystem = nullptr;
File TesLight::UpdateEndpoint::uploadFile = File();

/**
 * @brief Add all request handler for this {@link TesLight::RestEndpoint} to the {@link TesLight::WebServerManager}.
 */
void TesLight::UpdateEndpoint::begin(FS *_fileSystem)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize Update Endpoint."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Create storage directory for package file."));
	TesLight::UpdateEndpoint::fileSystem = _fileSystem;
	TesLight::UpdateEndpoint::fileSystem->mkdir(UPDATE_DIRECTORY);

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Register Update Endpoints."));
	webServerManager->addUploadRequestHandler((getBaseUri() + F("update")).c_str(), http_method::HTTP_POST, TesLight::UpdateEndpoint::postPackage, TesLight::UpdateEndpoint::packageUpload);
}

/**
 * @brief Is called after the update package upload.
 */
void TesLight::UpdateEndpoint::postPackage()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Package upload successful, update will start after reboot."));
	if (TesLight::UpdateEndpoint::uploadFile)
	{
		TesLight::UpdateEndpoint::uploadFile.close();
	}
	webServer->send(200, F("text/plain"), F("Package upload successful, update will start after reboot."));

	// Reboot the controller.
	// Update will be installed after the reboot.
	delay(1000);
	TesLight::Updater::reboot();
}

/**
 * @brief Upload a new update package to the controller.
 */
void TesLight::UpdateEndpoint::packageUpload()
{
	if (!TesLight::UpdateEndpoint::uploadFile)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to upload update package."));
		TesLight::UpdateEndpoint::uploadFile = TesLight::UpdateEndpoint::fileSystem->open((String)UPDATE_DIRECTORY + F("/") + UPDATE_FILE_NAME, FILE_WRITE);
		if (!uploadFile)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to write to file for upload."));
			webServer->send(500, F("text/plain"), F("Failed to write to file for upload."));
			return;
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Writing chunk of data."));
	HTTPUpload &upload = webServer->upload();
	if (TesLight::UpdateEndpoint::uploadFile.write(upload.buf, upload.currentSize) != upload.currentSize)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to write chunk to file. Not all bytes were written."));
	}
}
