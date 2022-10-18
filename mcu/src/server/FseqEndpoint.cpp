/**
 * @file FseqEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to manage fseq files on the TesLight controller.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "server/FseqEndpoint.h"

// Initialize
FS *TesLight::FseqEndpoint::fileSystem = nullptr;
File TesLight::FseqEndpoint::uploadFile = File();

/**
 * @brief Add all request handler for this {@link TesLight::RestEndpoint} to the {@link TesLight::WebServerManager}.
 */
void TesLight::FseqEndpoint::begin(FS *_fileSystem)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize Fseq Endpoint."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Create storage directory for fseq files."));
	TesLight::FseqEndpoint::fileSystem = _fileSystem;
	TesLight::FseqEndpoint::fileSystem->mkdir(FSEQ_DIRECTORY);

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Register Fseq Endpoints."));
	webServerManager->addRequestHandler((getBaseUri() + F("fseq")).c_str(), http_method::HTTP_GET, TesLight::FseqEndpoint::getFseqList);
	webServerManager->addUploadRequestHandler((getBaseUri() + F("fseq")).c_str(), http_method::HTTP_POST, TesLight::FseqEndpoint::postFseq, TesLight::FseqEndpoint::fseqUpload);
	webServerManager->addRequestHandler((getBaseUri() + F("fseq")).c_str(), http_method::HTTP_DELETE, TesLight::FseqEndpoint::deleteFseq);
}

/**
 * @brief Return a list of available fseq files on the controller.
 */
void TesLight::FseqEndpoint::getFseqList()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the fseq list."));

	String fileList;
	if (!TesLight::FileUtil::getFileList(fileSystem, FSEQ_DIRECTORY, fileList, false))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to list files."));
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	webServer->send(200, F("text/plain"), fileList);
}

/**
 * @brief Is called after the file upload of a fseq file.
 */
void TesLight::FseqEndpoint::postFseq()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Upload of fseq file completed."));
	if (TesLight::FseqEndpoint::uploadFile)
	{
		TesLight::FseqEndpoint::uploadFile.close();
	}
	webServer->send(200, F("text/plain"), F("File upload successful."));
}

/**
 * @brief Upload a new fseq files to the controller.
 */
void TesLight::FseqEndpoint::fseqUpload()
{
	if (!TesLight::FseqEndpoint::uploadFile)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to upload a new fseq file."));
		const String fileName = webServer->arg(F("fileName"));
		if (fileName.length() == 0)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Received request to upload a new fseq file but the fileName parameter is empty."));
			webServer->send(400, F("text/plain"), F("The fileName parameter must not be empty. Can not upload file."));
			return;
		}

		TesLight::FseqEndpoint::uploadFile = TesLight::FseqEndpoint::fileSystem->open((String)FSEQ_DIRECTORY + "/" + fileName, FILE_WRITE);
		if (!uploadFile)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to write to file for upload."));
			webServer->send(500, F("text/plain"), F("Failed to write to file for upload."));
			return;
		}
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Writing chunk of data."));
	HTTPUpload &upload = webServer->upload();
	if (TesLight::FseqEndpoint::uploadFile.write(upload.buf, upload.currentSize) != upload.currentSize)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to write chunk to file. Not all bytes were written."));
	}
}

/**
 * @brief Delete a fseq files from the controller.
 */
void TesLight::FseqEndpoint::deleteFseq()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to delete a fseq file."));

	const String fileName = webServer->arg(F("fileName"));
	if (fileName.length() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to delete fseq file because fileName parameter is empty."));
		webServer->send(400, F("text/plain"), F("Failed to delete fseq file because fileName parameter is empty."));
		return;
	}

	if (!fileSystem->exists(FSEQ_DIRECTORY + (String)F("/") + fileName))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("File ") + FSEQ_DIRECTORY + F("/") + fileName + F(" was not found."));
		webServer->send(404, F("text/plain"), (String)F("File ") + FSEQ_DIRECTORY + F("/") + fileName + F(" was not found."));
		return;
	}

	if (!fileSystem->remove(FSEQ_DIRECTORY + (String)F("/") + fileName))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to delete file."));
		webServer->send(500, F("text/plain"), F("Failed to delete file."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	webServer->send(200, F("text/plain"), F("File deleted."));
}