/**
 * @file FseqEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to manage fseq files on the TL controller.
 *
 * @copyright Copyright (c) 2022 TheRealKasumi
 *
 * This project, including hardware and software, is provided "as is". There is no warranty
 * of any kind, express or implied, including but not limited to the warranties of fitness
 * for a particular purpose and noninfringement. TheRealKasumi (https://github.com/TheRealKasumi)
 * is holding ownership of this project. You are free to use, modify, distribute and contribute
 * to this project for private, non-commercial purposes. It is granted to include this hardware
 * and software into private, non-commercial projects. However, the source code of any project,
 * software and hardware that is including this project must be public and free to use for private
 * persons. Any commercial use is hereby strictly prohibited without agreement from the owner.
 * By contributing to the project, you agree that the ownership of your work is transferred to
 * the project owner and that you lose any claim to your contribute work. This copyright and
 * license note applies to all files of this project and must not be removed without agreement
 * from the owner.
 *
 */
#include "server/FseqEndpoint.h"

// Initialize
FS *TL::FseqEndpoint::fileSystem = nullptr;
TL::Configuration *TL::FseqEndpoint::configuration = nullptr;
File TL::FseqEndpoint::uploadFile = File();

/**
 * @brief Add all request handler for this {@link TL::RestEndpoint} to the {@link TL::WebServerManager}.
 */
void TL::FseqEndpoint::begin(FS *_fileSystem, TL::Configuration *_configuration)
{
	TL::FseqEndpoint::fileSystem = _fileSystem;
	TL::FseqEndpoint::configuration = _configuration;
	TL::FseqEndpoint::fileSystem->mkdir(FSEQ_DIRECTORY);

	TL::FseqEndpoint::webServerManager->addRequestHandler((getBaseUri() + F("fseq")).c_str(), http_method::HTTP_GET, TL::FseqEndpoint::getFseqList);
	TL::FseqEndpoint::webServerManager->addUploadRequestHandler((getBaseUri() + F("fseq")).c_str(), http_method::HTTP_POST, TL::FseqEndpoint::postFseq, TL::FseqEndpoint::fseqUpload);
	TL::FseqEndpoint::webServerManager->addRequestHandler((getBaseUri() + F("fseq")).c_str(), http_method::HTTP_DELETE, TL::FseqEndpoint::deleteFseq);
}

/**
 * @brief Return a list of available fseq files on the controller.
 */
void TL::FseqEndpoint::getFseqList()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the fseq list."));
	DynamicJsonDocument jsonDoc(4096);
	JsonArray fileList = jsonDoc.createNestedArray(F("fileList"));

	if (!TL::FileUtil::listFiles(
			fileSystem, FSEQ_DIRECTORY, [jsonDoc, fileList](const String fileName, const size_t fileSize)
			{ 
				if(!jsonDoc.overflowed()) 
				{
					JsonObject object = fileList.createNestedObject();
					object[F("fileName")] = fileName;
					object[F("fileSize")] = fileSize;

					uint32_t fileId;
					if(TL::FileUtil::getFileIdentifier(fileSystem, FSEQ_DIRECTORY + (String)F("/") + fileName, fileId))
					{
						object[F("fileId")] = fileId;
					}
					else 
					{
						TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to get file identifier."));	
						object[F("fileId")] = 0;
					}
				} },
			false))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to list files."));
		TL::FseqEndpoint::sendSimpleResponse(500, F("The file list could not be read."));
		return;
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::FseqEndpoint::sendJsonDocument(200, F("Happily serving the file list to you :3 !"), jsonDoc);
}

/**
 * @brief Is called after the file upload of a fseq file.
 */
void TL::FseqEndpoint::postFseq()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Upload of fseq file completed."));
	TL::FseqEndpoint::sendSimpleResponse(200, F("File received! Can't wait to unpack it... Can I? Pleaaaase?"));
}

/**
 * @brief Upload a new fseq files to the controller.
 */
void TL::FseqEndpoint::fseqUpload()
{
	const String fileName = webServer->arg(F("fileName"));
	HTTPUpload &upload = webServer->upload();
	if (upload.status == UPLOAD_FILE_START)
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to upload a new fseq file."));
		if (fileName.length() == 0)
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The fileName parameter must not be empty. Can not upload file."));
			TL::FseqEndpoint::sendSimpleResponse(400, F("The fileName parameter must not be empty. Can not upload file."));
			return;
		}
		else if (!TL::FseqEndpoint::validateFileName(fileName))
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The received file name is invalid."));
			TL::FseqEndpoint::sendSimpleResponse(400, F("The received file name is invalid."));
			return;
		}

		if (TL::FileUtil::fileExists(TL::FseqEndpoint::fileSystem, (String)FSEQ_DIRECTORY + F("/") + fileName))
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("A file with name \"") + fileName + F("\" already exists."));
			TL::FseqEndpoint::sendSimpleResponse(409, (String)F("A file with name \"") + fileName + F("\" already exists."));
			return;
		}

		TL::FseqEndpoint::uploadFile = TL::FseqEndpoint::fileSystem->open((String)FSEQ_DIRECTORY + F("/") + fileName, FILE_WRITE);
		if (!uploadFile)
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to write to file for upload."));
			TL::FseqEndpoint::sendSimpleResponse(500, F("Failed to write to file for upload."));
			return;
		}
	}
	else if (upload.status == UPLOAD_FILE_WRITE && TL::FseqEndpoint::uploadFile)
	{
		if (TL::FseqEndpoint::uploadFile.write(upload.buf, upload.currentSize) != upload.currentSize)
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to write chunk to file. Not all bytes were written."));
			TL::FseqEndpoint::sendSimpleResponse(500, F("Failed to write chunk to file. Not all bytes were written."));
			return;
		}
	}
	else if (upload.status == UPLOAD_FILE_END)
	{
		TL::FseqEndpoint::uploadFile.close();
		if (!TL::FseqEndpoint::validateFseqFile((String)FSEQ_DIRECTORY + F("/") + fileName))
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The uploaded fseq file is invalid and will be deleted."));
			TL::FseqEndpoint::fileSystem->remove((String)FSEQ_DIRECTORY + F("/") + fileName);
			TL::FseqEndpoint::sendSimpleResponse(400, F("The uploaded fseq file is invalid and will be deleted."));
			return;
		}
	}
	else if (upload.status == UPLOAD_FILE_ABORTED)
	{
		if (TL::FseqEndpoint::uploadFile)
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Upload was aborted, file will be deleted."));
			TL::FseqEndpoint::uploadFile.close();
			TL::FseqEndpoint::fileSystem->remove((String)FSEQ_DIRECTORY + F("/") + fileName);
			TL::FseqEndpoint::sendSimpleResponse(400, F("Upload was aborted by the client. The data was dropped."));
		}
	}
}

/**
 * @brief Delete a fseq files from the controller.
 */
void TL::FseqEndpoint::deleteFseq()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to delete a fseq file."));
	const String fileName = webServer->arg(F("fileName"));
	if (fileName.length() == 0)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to delete fseq file because file name parameter is empty."));
		TL::FseqEndpoint::sendSimpleResponse(400, F("Failed to delete fseq file because the file name parameter is empty."));
		return;
	}

	if (!fileSystem->exists(FSEQ_DIRECTORY + (String)F("/") + fileName))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("File ") + FSEQ_DIRECTORY + F("/") + fileName + F(" was not found."));
		TL::FseqEndpoint::sendSimpleResponse(404, (String)F("File ") + FSEQ_DIRECTORY + F("/") + fileName + F(" was not found."));
		return;
	}

	if (TL::FseqEndpoint::configuration->getLedConfig(0).type == 255)
	{
		uint32_t idFile = 0;
		uint32_t idConfig = 0;
		if (!TL::FileUtil::getFileIdentifier(TL::FseqEndpoint::fileSystem, FSEQ_DIRECTORY + (String)F("/") + fileName, idFile))
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to calculate file identifier."));
			TL::FseqEndpoint::sendSimpleResponse(500, F("Failed to calculate file identifier."));
			return;
		}
		memcpy(&idConfig, &TL::FseqEndpoint::configuration->getLedConfig(0).animationSettings[20], sizeof(idConfig));
		if (idFile == idConfig)
		{
			TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Can not delete a fseq file that is currently used."));
			TL::FseqEndpoint::sendSimpleResponse(400, F("Can not delete a fseq file that is currently used."));
			return;
		}
	}

	if (!fileSystem->remove(FSEQ_DIRECTORY + (String)F("/") + fileName))
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to delete file."));
		TL::FseqEndpoint::sendSimpleResponse(500, F("Failed to delete file."));
		return;
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::FseqEndpoint::sendSimpleResponse(200, F("File deleted."));
}

/**
 * @brief Validate the file name and check for invalid characters.
 * @param fileName received name of the file
 * @return true when valid
 * @return false when invalid
 */
bool TL::FseqEndpoint::validateFileName(const String fileName)
{
	if (fileName.length() < 1)
	{
		return false;
	}
	else if (fileName.length() > 32)
	{
		return false;
	}

	for (unsigned int i = 0; i < fileName.length(); i++)
	{
		if ((fileName[i] < 'A' || fileName[i] > 'Z') && (fileName[i] < 'a' || fileName[i] > 'z') && (fileName[i] < '0' || fileName[i] > '9') && fileName[i] != '.' && fileName[i] != '_' && fileName[i] != '-' && fileName[i] != ' ')
		{
			return false;
		}
	}

	return true;
}

/**
 * @brief Validate a fseq file by using the {@link TL::FseqLoader}.
 * @param fileName full path and name of the file to check
 * @return true when valid
 * @return false when invalid
 */
bool TL::FseqEndpoint::validateFseqFile(const String fileName)
{
	TL::FseqLoader fseqLoader(TL::FseqEndpoint::fileSystem);
	const bool valid = fseqLoader.loadFromFile(fileName);
	fseqLoader.close();
	return valid;
}