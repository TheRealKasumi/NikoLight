/**
 * @file FseqEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to manage fseq files on the TL controller.
 *
 * @copyright Copyright (c) 2022-2023 TheRealKasumi
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
FS *NL::FseqEndpoint::fileSystem = nullptr;
File NL::FseqEndpoint::uploadFile = File();

/**
 * @brief Add all request handler for this {@link NL::RestEndpoint} to the {@link NL::WebServerManager}.
 */
void NL::FseqEndpoint::begin(FS *_fileSystem)
{
	NL::FseqEndpoint::fileSystem = _fileSystem;
	NL::FseqEndpoint::fileSystem->mkdir(FSEQ_DIRECTORY);

	NL::WebServerManager::addRequestHandler((getBaseUri() + F("fseq")).c_str(), http_method::HTTP_GET, NL::FseqEndpoint::getFseqList);
	NL::WebServerManager::addUploadRequestHandler((getBaseUri() + F("fseq")).c_str(), http_method::HTTP_POST, NL::FseqEndpoint::postFseq, NL::FseqEndpoint::fseqUpload);
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("fseq")).c_str(), http_method::HTTP_DELETE, NL::FseqEndpoint::deleteFseq);
}

/**
 * @brief Return a list of available fseq files on the controller.
 */
void NL::FseqEndpoint::getFseqList()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the fseq list."));
	DynamicJsonDocument jsonDoc(4096);
	const JsonArray fileList = jsonDoc.createNestedArray(F("fileList"));

	if (!NL::FileUtil::listFiles(
			fileSystem, FSEQ_DIRECTORY, [jsonDoc, fileList](const String fileName, const size_t fileSize)
			{ 
				if(!jsonDoc.overflowed()) 
				{
					JsonObject object = fileList.createNestedObject();
					object[F("fileName")] = fileName;
					object[F("fileSize")] = fileSize;

					uint32_t fileId;
					if(NL::FileUtil::getFileIdentifier(fileSystem, FSEQ_DIRECTORY + (String)F("/") + fileName, fileId))
					{
						object[F("fileId")] = fileId;
					}
					else 
					{
						NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to get file identifier."));	
						object[F("fileId")] = 0;
					}
				} },
			false))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to list files."));
		NL::FseqEndpoint::sendSimpleResponse(500, F("The file list could not be read."));
		return;
	}

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	NL::FseqEndpoint::sendJsonDocument(200, F("Happily serving the file list to you :3 !"), jsonDoc);
}

/**
 * @brief Is called after the file upload of a fseq file.
 */
void NL::FseqEndpoint::postFseq()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Upload of fseq file completed."));
	NL::FseqEndpoint::sendSimpleResponse(200, F("File received! Can't wait to unpack it... Can I? Pleaaaase?"));
}

/**
 * @brief Upload a new fseq files to the controller.
 */
void NL::FseqEndpoint::fseqUpload()
{
	const String fileName = webServer->arg(F("fileName"));
	HTTPUpload &upload = webServer->upload();
	if (upload.status == UPLOAD_FILE_START)
	{
		NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to upload a new fseq file."));
		if (fileName.length() == 0)
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The fileName parameter must not be empty. Can not upload file."));
			NL::FseqEndpoint::sendSimpleResponse(400, F("The fileName parameter must not be empty. Can not upload file."));
			return;
		}
		else if (!NL::FseqEndpoint::validateFileName(fileName))
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The received file name is invalid."));
			NL::FseqEndpoint::sendSimpleResponse(400, F("The received file name is invalid."));
			return;
		}

		if (NL::FileUtil::fileExists(NL::FseqEndpoint::fileSystem, (String)FSEQ_DIRECTORY + F("/") + fileName))
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("A file with name \"") + fileName + F("\" already exists."));
			NL::FseqEndpoint::sendSimpleResponse(409, (String)F("A file with name \"") + fileName + F("\" already exists."));
			return;
		}

		NL::FseqEndpoint::uploadFile = NL::FseqEndpoint::fileSystem->open((String)FSEQ_DIRECTORY + F("/") + fileName, FILE_WRITE);
		if (!uploadFile)
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to write to file for upload."));
			NL::FseqEndpoint::sendSimpleResponse(500, F("Failed to write to file for upload."));
			return;
		}
	}
	else if (upload.status == UPLOAD_FILE_WRITE && NL::FseqEndpoint::uploadFile)
	{
		if (NL::FseqEndpoint::uploadFile.write(upload.buf, upload.currentSize) != upload.currentSize)
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to write chunk to file. Not all bytes were written."));
			NL::FseqEndpoint::sendSimpleResponse(500, F("Failed to write chunk to file. Not all bytes were written."));
			return;
		}
	}
	else if (upload.status == UPLOAD_FILE_END)
	{
		NL::FseqEndpoint::uploadFile.close();
		NL::FseqLoader::Error fseqError = NL::FseqEndpoint::validateFseqFile((String)FSEQ_DIRECTORY + F("/") + fileName);

		if (fseqError == NL::FseqLoader::Error::ERROR_FILE_TOO_SMALL)
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The uploaded fseq file is invalid because it is too small. File will be deleted."));
			NL::FseqEndpoint::fileSystem->remove((String)FSEQ_DIRECTORY + F("/") + fileName);
			NL::FseqEndpoint::sendSimpleResponse(400, F("The uploaded fseq file is invalid because it is too small. File will be deleted."));
			return;
		}
		else if (fseqError == NL::FseqLoader::Error::ERROR_MAGIC_NUMBERS)
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The uploaded fseq file is not a valid fseq file. File will be deleted."));
			NL::FseqEndpoint::fileSystem->remove((String)FSEQ_DIRECTORY + F("/") + fileName);
			NL::FseqEndpoint::sendSimpleResponse(400, F("The uploaded fseq file is not a valid fseq file. File will be deleted."));
			return;
		}
		else if (fseqError == NL::FseqLoader::Error::ERROR_FILE_VERSION)
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The uploaded fseq file has a invalid version. File will be deleted."));
			NL::FseqEndpoint::fileSystem->remove((String)FSEQ_DIRECTORY + F("/") + fileName);
			NL::FseqEndpoint::sendSimpleResponse(400, F("The uploaded fseq file has a invalid version. File will be deleted."));
			return;
		}
		else if (fseqError == NL::FseqLoader::Error::ERROR_HEADER_LENGTH)
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The uploaded fseq file has a invalid header length. File will be deleted."));
			NL::FseqEndpoint::fileSystem->remove((String)FSEQ_DIRECTORY + F("/") + fileName);
			NL::FseqEndpoint::sendSimpleResponse(400, F("The uploaded fseq file has a invalid header length. File will be deleted."));
			return;
		}
		else if (fseqError == NL::FseqLoader::Error::ERROR_INVALID_DATA_LENGTH)
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The uploaded fseq file has a invalid data length. File will be deleted."));
			NL::FseqEndpoint::fileSystem->remove((String)FSEQ_DIRECTORY + F("/") + fileName);
			NL::FseqEndpoint::sendSimpleResponse(400, F("The uploaded fseq file has a invalid data length. File will be deleted."));
			return;
		}
		else if (fseqError != NL::FseqLoader::Error::OK)
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The uploaded fseq file is invalid and will be deleted."));
			NL::FseqEndpoint::fileSystem->remove((String)FSEQ_DIRECTORY + F("/") + fileName);
			NL::FseqEndpoint::sendSimpleResponse(400, F("The uploaded fseq file is invalid and will be deleted."));
			return;
		}
	}
	else if (upload.status == UPLOAD_FILE_ABORTED)
	{
		if (NL::FseqEndpoint::uploadFile)
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Upload was aborted, file will be deleted."));
			NL::FseqEndpoint::uploadFile.close();
			NL::FseqEndpoint::fileSystem->remove((String)FSEQ_DIRECTORY + F("/") + fileName);
			NL::FseqEndpoint::sendSimpleResponse(400, F("Upload was aborted by the client. The data was dropped."));
		}
	}
}

/**
 * @brief Delete a fseq files from the controller.
 */
void NL::FseqEndpoint::deleteFseq()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to delete a fseq file."));
	const String fileName = webServer->arg(F("fileName"));
	if (fileName.length() == 0)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to delete fseq file because file name parameter is empty."));
		NL::FseqEndpoint::sendSimpleResponse(400, F("Failed to delete fseq file because the file name parameter is empty."));
		return;
	}

	if (!fileSystem->exists(FSEQ_DIRECTORY + (String)F("/") + fileName))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, (String)F("File ") + FSEQ_DIRECTORY + F("/") + fileName + F(" was not found."));
		NL::FseqEndpoint::sendSimpleResponse(404, (String)F("File ") + FSEQ_DIRECTORY + F("/") + fileName + F(" was not found."));
		return;
	}

	NL::Configuration::LedConfig ledConfig;
	NL::Configuration::getLedConfig(0, ledConfig);
	if (ledConfig.type == 255)
	{
		uint32_t idFile = 0;
		uint32_t idConfig = 0;
		if (!NL::FileUtil::getFileIdentifier(NL::FseqEndpoint::fileSystem, FSEQ_DIRECTORY + (String)F("/") + fileName, idFile))
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to calculate file identifier."));
			NL::FseqEndpoint::sendSimpleResponse(500, F("Failed to calculate file identifier."));
			return;
		}

		memcpy(&idConfig, &ledConfig.animationSettings[20], sizeof(idConfig));
		if (idFile == idConfig)
		{
			NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Can not delete a fseq file that is currently used."));
			NL::FseqEndpoint::sendSimpleResponse(400, F("Can not delete a fseq file that is currently used."));
			return;
		}
	}

	if (!fileSystem->remove(FSEQ_DIRECTORY + (String)F("/") + fileName))
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to delete file."));
		NL::FseqEndpoint::sendSimpleResponse(500, F("Failed to delete file."));
		return;
	}

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	NL::FseqEndpoint::sendSimpleResponse(200, F("File deleted."));
}

/**
 * @brief Validate the file name and check for invalid characters.
 * @param fileName received name of the file
 * @return true when valid
 * @return false when invalid
 */
bool NL::FseqEndpoint::validateFileName(const String fileName)
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
 * @brief Validate a fseq file by using the {@link NL::FseqLoader}.
 * @param fileName full path and name of the file to check
 * @return NL::FseqLoader::OK when the file was loaded and is valid
 * @return NL::FseqLoader::ERROR_FILE_NOT_FOUND when the file was not found
 * @return NL::FseqLoader::ERROR_FILE_IS_DIR when the file is a directory
 * @return NL::FseqLoader::ERROR_FILE_TOO_SMALL when the file is too small to be valid
 * @return NL::FseqLoader::ERROR_FILE_READ when the file could not be read
 * @return NL::FseqLoader::ERROR_MAGIC_NUMBERS when the magic numbers do not match
 * @return NL::FseqLoader::ERROR_FILE_VERSION  when the file version is unsupported
 * @return NL::FseqLoader::ERROR_HEADER_LENGTH when the header length is invalid
 * @return NL::FseqLoader::ERROR_INVALID_DATA_LENGTH when the data length does not match the length specified in header
 */
NL::FseqLoader::Error NL::FseqEndpoint::validateFseqFile(const String fileName)
{
	NL::FseqLoader fseqLoader(NL::FseqEndpoint::fileSystem);
	const NL::FseqLoader::Error fseqError = fseqLoader.loadFromFile(fileName);
	fseqLoader.close();
	return fseqError;
}