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
 * @brief Add all request handler for this {@link TesLight::RestEndpoint} to the {@link TesLight::WebServer}.
 */
void TesLight::FseqEndpoint::begin(FS *_fileSystem)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize Fseq Endpoint."));

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Create storage directory for fseq files."));
	fileSystem = _fileSystem;
	fileSystem->mkdir(FSEQ_DIRECTORY);

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Register Fseq Endpoints."));
	getWebServer()->addRequestHandler((getBaseUri() + F("fseq")).c_str(), http_method::HTTP_GET, TesLight::FseqEndpoint::getFseqList);
	getWebServer()->addRequestBodyHandler((getBaseUri() + F("fseq")).c_str(), http_method::HTTP_POST, TesLight::FseqEndpoint::postFseq, TesLight::FseqEndpoint::postFseqBody);
	// TODO: Why delete method isn't working? Bug in library or expected? Until resolved patch is used for deleting... :(
	getWebServer()->addRequestHandler((getBaseUri() + F("fseq")).c_str(), http_method::HTTP_PATCH, TesLight::FseqEndpoint::deleteFseq);
}

/**
 * @brief Return a list of available fseq files on the controller.
 * @param request instance of {@link AsyncWebServerRequest}
 */
void TesLight::FseqEndpoint::getFseqList(AsyncWebServerRequest *request)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the fseq list."));

	String fileList;
	if (!TesLight::FileUtil::getFileList(fileSystem, FSEQ_DIRECTORY, fileList))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to list files."));
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	request->send(200, F("text/plain"), fileList);
}

/**
 * @brief Is called after the file upload of a fseq file.
 * @param request instance of {@link AsyncWebServerRequest}
 */
void TesLight::FseqEndpoint::postFseq(AsyncWebServerRequest *request)
{
	request->send(200, F("text/plain"), F("File upload successful."));
}

/**
 * @brief Upload a new fseq files to the controller.
 * @param request instance of {@link AsyncWebServerRequest}
 * @param data chunk of data
 * @param len length of the data chunk
 * @param index start index of the data chunk
 * @param total total size of the upload
 */
void TesLight::FseqEndpoint::postFseqBody(AsyncWebServerRequest *request, uint8_t *data, const size_t len, const size_t index, const size_t total)
{
	const String fileName = request->arg("fileName");
	if (fileName.length() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Received request to upload a new fseq file but the fileName parameter is empty."));
		request->send(400, F("text/plain"), F("The fileName parameter must not be empty. Can not upload file."));
		return;
	}

	if (index == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Received request to upload file ") + fileName + F(" with a total size of ") + String(total / 1024.0f) + F(" kB."));
		if (!uploadFile)
		{
			uploadFile = fileSystem->open((String)FSEQ_DIRECTORY + "/" + fileName, FILE_WRITE);
			if (!uploadFile)
			{
				TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open file for upload."));
				request->send(500, F("text/plain"), F("Failed to open file for upload."));
				return;
			}
		}
		else
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Upload is already in progress. Can not start a parallel upload."));
			request->send(400, F("text/plain"), F("Upload is already in progress. Can not start a parallel upload."));
			return;
		}
	}

	if (uploadFile.write(data, len) != len)
	{
		uploadFile.close();
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to write chunk to the file."));
		request->send(500, F("text/plain"), F("Failed to write chunk to the file."));
		return;
	}

	if (index + len == total)
	{
		uploadFile.close();
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Upload complete, file saved."));
	}
}

/**
 * @brief Delete a fseq files from the controller.
 * @param request instance of {@link AsyncWebServerRequest}
 */
void TesLight::FseqEndpoint::deleteFseq(AsyncWebServerRequest *request)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to delete a fseq file."));

	const String fileName = request->arg("fileName");
	if (fileName.length() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to delete file because fileName parameter is empty."));
		request->send(400, F("text/plain"), F("Failed to delete file because fileName parameter is empty."));
		return;
	}

	if (!fileSystem->exists(FSEQ_DIRECTORY + (String)F("/") + fileName))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("File ") + FSEQ_DIRECTORY + F("/") + fileName + F(" was not found."));
		request->send(404, F("text/plain"), (String)F("File ") + FSEQ_DIRECTORY + F("/") + fileName + F(" was not found."));
		return;
	}

	if (!fileSystem->remove(FSEQ_DIRECTORY + (String)F("/") + fileName))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Failed to delete file."));
		request->send(500, F("text/plain"), F("Failed to delete file."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	request->send(200, F("text/plain"), F("File deleted."));
}