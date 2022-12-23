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
FS *TL::UpdateEndpoint::fileSystem = nullptr;
File TL::UpdateEndpoint::uploadFile = File();

/**
 * @brief Add all request handler for this {@link TL::RestEndpoint} to the {@link TL::WebServerManager}.
 */
void TL::UpdateEndpoint::begin(FS *_fileSystem)
{
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize update endpoint."));

	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Create storage directory for package file."));
	TL::UpdateEndpoint::fileSystem = _fileSystem;
	TL::UpdateEndpoint::fileSystem->mkdir(UPDATE_DIRECTORY);

	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Register update endpoints."));
	TL::UpdateEndpoint::webServerManager->addUploadRequestHandler((getBaseUri() + F("update")).c_str(), http_method::HTTP_POST, TL::UpdateEndpoint::postPackage, TL::UpdateEndpoint::packageUpload);
}

/**
 * @brief Is called after the update package upload.
 */
void TL::UpdateEndpoint::postPackage()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Package upload successful, update will start after reboot."));
	TL::UpdateEndpoint::sendSimpleResponse(200, F("Yay, I received a package. I will unpack it. Can't wait to see what's inside."));

	if (TL::UpdateEndpoint::uploadFile)
	{
		TL::UpdateEndpoint::uploadFile.close();
	}

	// Reboot the controller, update will be installed after the reboot
	TL::Updater::reboot(F("Update"), 3000);
}

/**
 * @brief Upload a new update package to the controller.
 */
void TL::UpdateEndpoint::packageUpload()
{
	HTTPUpload &upload = webServer->upload();
	if (upload.status == UPLOAD_FILE_START)
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to upload update package."));
		TL::UpdateEndpoint::uploadFile = TL::UpdateEndpoint::fileSystem->open((String)UPDATE_DIRECTORY + F("/") + UPDATE_FILE_NAME, FILE_WRITE);
		if (!uploadFile)
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to write to file for upload."));
			TL::UpdateEndpoint::sendSimpleResponse(500, F("Failed to write to file for upload."));
			return;
		}
	}
	else if (upload.status == UPLOAD_FILE_WRITE)
	{
		if (TL::UpdateEndpoint::uploadFile.write(upload.buf, upload.currentSize) != upload.currentSize)
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to write chunk to file. Not all bytes were written."));
			TL::UpdateEndpoint::sendSimpleResponse(500, F("Failed to write chunk to file. Not all bytes were written."));
			return;
		}
	}
	else if (upload.status == UPLOAD_FILE_END)
	{
		TL::UpdateEndpoint::uploadFile.close();
	}
	else if (upload.status == UPLOAD_FILE_ABORTED)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The upload was aborted by the client."));
		TL::UpdateEndpoint::uploadFile.close();
		TL::UpdateEndpoint::sendSimpleResponse(400, F("Upload was aborted by the client."));
	}
}
