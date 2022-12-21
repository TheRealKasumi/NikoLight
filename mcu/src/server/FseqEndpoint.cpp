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
TesLight::Configuration *TesLight::FseqEndpoint::configuration = nullptr;
File TesLight::FseqEndpoint::uploadFile = File();

/**
 * @brief Add all request handler for this {@link TesLight::RestEndpoint} to the {@link TesLight::WebServerManager}.
 */
void TesLight::FseqEndpoint::begin(FS *_fileSystem, TesLight::Configuration *_configuration)
{
	TesLight::FseqEndpoint::fileSystem = _fileSystem;
	TesLight::FseqEndpoint::configuration = _configuration;
	TesLight::FseqEndpoint::fileSystem->mkdir(FSEQ_DIRECTORY);

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
	webServer->send(200, F("text/plain"), F("File upload successful."));
}

/**
 * @brief Upload a new fseq files to the controller.
 */
void TesLight::FseqEndpoint::fseqUpload()
{
	const String fileName = webServer->arg(F("fileName"));
	HTTPUpload &upload = webServer->upload();
	if (upload.status == UPLOAD_FILE_START)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to upload a new fseq file."));
		if (fileName.length() == 0)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The fileName parameter must not be empty. Can not upload file."));
			webServer->send(400, F("text/plain"), F("The fileName parameter must not be empty. Can not upload file."));
			return;
		}
		else if (!TesLight::FseqEndpoint::verifyFileName(fileName))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The received file name is invalid."));
			webServer->send(400, F("text/plain"), F("The received file name is invalid."));
			return;
		}

		if (TesLight::FileUtil::fileExists(TesLight::FseqEndpoint::fileSystem, (String)FSEQ_DIRECTORY + F("/") + fileName))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("A file with name \"") + fileName + F("\" already exists."));
			webServer->send(409, F("text/plain"), (String)F("A file with name \"") + fileName + F("\" already exists."));
			return;
		}

		TesLight::FseqEndpoint::uploadFile = TesLight::FseqEndpoint::fileSystem->open((String)FSEQ_DIRECTORY + F("/") + fileName, FILE_WRITE);
		if (!uploadFile)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to write to file for upload."));
			webServer->send(500, F("text/plain"), F("Failed to write to file for upload."));
			return;
		}
	}
	else if (upload.status == UPLOAD_FILE_WRITE)
	{
		if (TesLight::FseqEndpoint::uploadFile.write(upload.buf, upload.currentSize) != upload.currentSize)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to write chunk to file. Not all bytes were written."));
			webServer->send(500, F("text/plain"), F("Failed to write chunk to file. Not all bytes were written."));
			return;
		}
	}
	else if (upload.status == UPLOAD_FILE_END)
	{
		TesLight::FseqEndpoint::uploadFile.close();
		if (!TesLight::FseqEndpoint::verifyFseqFile((String)FSEQ_DIRECTORY + F("/") + fileName))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The uploaded fseq file is invalid and will not be deleted."));
			TesLight::FseqEndpoint::fileSystem->remove((String)FSEQ_DIRECTORY + F("/") + fileName);
			webServer->send(400, F("text/plain"), F("The uploaded fseq file is invalid and will be deleted."));
			return;
		}
	}
	else if (upload.status == UPLOAD_FILE_ABORTED)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Upload was aborted, file will be deleted."));
		TesLight::FseqEndpoint::uploadFile.close();
		TesLight::FseqEndpoint::fileSystem->remove((String)FSEQ_DIRECTORY + F("/") + fileName);
		webServer->send(400, F("text/plain"), F("Upload was aborted by the client. The data was dropped."));
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
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to delete fseq file because file name parameter is empty."));
		webServer->send(400, F("text/plain"), F("Failed to delete fseq file because the file name parameter is empty."));
		return;
	}

	if (!fileSystem->exists(FSEQ_DIRECTORY + (String)F("/") + fileName))
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("File ") + FSEQ_DIRECTORY + F("/") + fileName + F(" was not found."));
		webServer->send(404, F("text/plain"), (String)F("File ") + FSEQ_DIRECTORY + F("/") + fileName + F(" was not found."));
		return;
	}

	if (TesLight::FseqEndpoint::configuration->getLedConfig(0).type == 255)
	{
		uint32_t idFile = 0;
		uint32_t idConfig = 0;
		if (!TesLight::FileUtil::getFileIdentifier(TesLight::FseqEndpoint::fileSystem, FSEQ_DIRECTORY + (String)F("/") + fileName, idFile))
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to calculate file identifier."));
			webServer->send(500, F("text/plain"), F("Failed to calculate file identifier."));
			return;
		}
		memcpy(&idConfig, &TesLight::FseqEndpoint::configuration->getLedConfig(0).customField[10], sizeof(idConfig));
		if (idFile == idConfig)
		{
			TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Can not delete a fseq file that is currently used."));
			webServer->send(400, F("text/plain"), F("Can not delete a fseq file that is currently used."));
			return;
		}
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

/**
 * @brief Verify the file name and check for invalid characters.
 * @param fileName received name of the file
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::FseqEndpoint::verifyFileName(const String fileName)
{
	if (fileName.length() < 1)
	{
		return false;
	}
	else if (fileName.length() > 32)
	{
		return false;
	}

	for (uint16_t i = 0; i < fileName.length(); i++)
	{
		if ((fileName[i] < 'A' || fileName[i] > 'Z') && (fileName[i] < 'a' || fileName[i] > 'z') && (fileName[i] < '0' || fileName[i] > '9') && fileName[i] != '.' && fileName[i] != '_' && fileName[i] != '-' && fileName[i] != ' ')
		{
			return false;
		}
	}

	return true;
}

/**
 * @brief Verify a fseq file by using the {@link TesLight::FseqLoader}.
 * @param fileName full path and name of the file to check
 * @return true when valid
 * @return false when invalid
 */
bool TesLight::FseqEndpoint::verifyFseqFile(const String fileName)
{
	TesLight::FseqLoader fseqLoader(TesLight::FseqEndpoint::fileSystem);
	const bool valid = fseqLoader.loadFromFile(fileName);
	fseqLoader.close();
	return valid;
}