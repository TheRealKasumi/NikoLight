/**
 * @file LogEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to manage the log of the TesLight controller.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "server/LogEndpoint.h"

// Initialize
FS *TL::LogEndpoint::fileSystem = nullptr;

/**
 * @brief Add all request handler for this {@link TL::RestEndpoint} to the {@link TL::WebServerManager}.
 */
void TL::LogEndpoint::begin(FS *_fileSystem)
{
	TL::LogEndpoint::fileSystem = _fileSystem;
	TL::LogEndpoint::webServerManager->addRequestHandler((getBaseUri() + F("log/size")).c_str(), http_method::HTTP_GET, TL::LogEndpoint::getLogSize);
	TL::LogEndpoint::webServerManager->addRequestHandler((getBaseUri() + F("log")).c_str(), http_method::HTTP_GET, TL::LogEndpoint::getLog);
	TL::LogEndpoint::webServerManager->addRequestHandler((getBaseUri() + F("log")).c_str(), http_method::HTTP_DELETE, TL::LogEndpoint::clearLog);
}

/**
 * @brief Return the size of the log file in bytes.
 */
void TL::LogEndpoint::getLogSize()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the log size."));
	File file = TL::LogEndpoint::fileSystem->open(LOG_FILE_NAME, FILE_READ);
	if (!file)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open log file."));
		TL::LogEndpoint::sendSimpleResponse(500, F("Failed to open log file."));
		return;
	}
	else if (file.isDirectory())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open log file because it is a directory."));
		file.close();
		TL::LogEndpoint::sendSimpleResponse(500, F("Failed to open log file because it is a directory."));
		return;
	}

	const size_t logSize = file.size();
	file.close();

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	DynamicJsonDocument jsonDoc(1024);
	JsonObject log = jsonDoc.createNestedObject(F("log"));
	log[F("size")] = logSize;
	TL::LogEndpoint::sendJsonDocument(200, F("This is my current log size."), jsonDoc);
}

/**
 * @brief Get a section of the log file, determinded by the paremters start and count in bytes.
 */
void TL::LogEndpoint::getLog()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get a section of the log file."));
	if (!TL::LogEndpoint::webServer->hasArg(F("start")) || TL::LogEndpoint::webServer->arg(F("start")).length() == 0 || !TL::LogEndpoint::webServer->hasArg(F("count")) || TL::LogEndpoint::webServer->arg(F("count")).length() == 0)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The parameters \"start\" and \"count\" must be provided."));
		TL::LogEndpoint::sendSimpleResponse(400, F("The url parameters \"start\" and \"count\" must be provided."));
		return;
	}

	File file = TL::LogEndpoint::fileSystem->open(LOG_FILE_NAME, FILE_READ);
	if (!file)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open log file."));
		TL::LogEndpoint::sendSimpleResponse(500, F("Failed to open log file."));
		return;
	}
	else if (file.isDirectory())
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open log file because it is a directory."));
		file.close();
		TL::LogEndpoint::sendSimpleResponse(500, F("Failed to open log file because it is a directory."));
		return;
	}

	const size_t start = TL::LogEndpoint::webServer->arg(F("start")).toInt();
	const size_t count = TL::LogEndpoint::webServer->arg(F("count")).toInt();
	const size_t logSize = file.size();
	if (start > logSize || start + count > logSize)
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The start or count parameters are invalid."));
		file.close();
		TL::LogEndpoint::sendSimpleResponse(400, F("The start or count parameters are invalid."));
		return;
	}

	TL::LogEndpoint::webServer->setContentLength(count);
	TL::LogEndpoint::webServer->send(200, F("text/plain"), F(""));

	file.seek(start);
	uint8_t buffer[512];
	size_t sentBytes = 0;
	while (sentBytes < count)
	{
		size_t chunkSize = count - sentBytes;
		if (chunkSize > 512)
		{
			chunkSize = 512;
		}
		chunkSize = file.read(buffer, chunkSize);
		TL::LogEndpoint::webServer->sendContent_P((char *)buffer, chunkSize);
		sentBytes += chunkSize;
	}

	file.close();
}

/**
 * @brief Clear the log file of the controller.
 */
void TL::LogEndpoint::clearLog()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to clear the log file."));
	TL::Logger::clearLog();
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	TL::LogEndpoint::sendSimpleResponse(200, F("All clean. Can I now have a cookie please?"));
}