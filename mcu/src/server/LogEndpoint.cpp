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
FS *TesLight::LogEndpoint::fileSystem = nullptr;

/**
 * @brief Add all request handler for this {@link TesLight::RestEndpoint} to the {@link TesLight::WebServerManager}.
 */
void TesLight::LogEndpoint::begin(FS *_fileSystem)
{
	TesLight::LogEndpoint::fileSystem = _fileSystem;
	webServerManager->addRequestHandler((getBaseUri() + F("log/size")).c_str(), http_method::HTTP_GET, TesLight::LogEndpoint::getLogSize);
	webServerManager->addRequestHandler((getBaseUri() + F("log")).c_str(), http_method::HTTP_GET, TesLight::LogEndpoint::getLog);
	webServerManager->addRequestHandler((getBaseUri() + F("log")).c_str(), http_method::HTTP_DELETE, TesLight::LogEndpoint::clearLog);
}

/**
 * @brief Return the size of the log file in bytes.
 */
void TesLight::LogEndpoint::getLogSize()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the log size."));
	File file = TesLight::LogEndpoint::fileSystem->open(LOG_FILE_NAME, FILE_READ);
	if (!file)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open log file."));
		webServer->send(500, F("text/plain"), F("Failed to open log file."));
		return;
	}
	else if (file.isDirectory())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open log file because it is a directory."));
		file.close();
		webServer->send(500, F("text/plain"), F("Failed to open log file because it is a directory."));
		return;
	}

	const size_t logSize = file.size();
	file.close();

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	webServer->send(200, F("text/plain"), String(logSize));
}

/**
 * @brief Get a section of the log file, determinded by the paremters start and count in bytes.
 */
void TesLight::LogEndpoint::getLog()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get a section of the log file."));
	if (!webServer->hasArg(F("start")) || webServer->arg(F("start")).length() == 0 || !webServer->hasArg(F("count")) || webServer->arg(F("count")).length() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The parameters \"start\" and \"count\" must be provided."));
		webServer->send(400, F("text/plain"), F("The parameters \"start\" and \"count\" must be provided."));
		return;
	}

	File file = TesLight::LogEndpoint::fileSystem->open(LOG_FILE_NAME, FILE_READ);
	if (!file)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open log file."));
		webServer->send(500, F("text/plain"), F("Failed to open log file."));
		return;
	}
	else if (file.isDirectory())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open log file because it is a directory."));
		file.close();
		webServer->send(500, F("text/plain"), F("Failed to open log file because it is a directory."));
		return;
	}

	const size_t start = webServer->arg(F("start")).toInt();
	const size_t count = webServer->arg(F("count")).toInt();
	const size_t logSize = file.size();
	if (start > logSize || start + count > logSize)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The start or count parameters are invalid."));
		file.close();
		webServer->send(400, F("text/plain"), F("The start or count parameters are invalid."));
		return;
	}

	webServer->setContentLength(count);
	webServer->send(200, F("text/plain"), F(""));

	file.seek(start);
	uint8_t buffer[1024];
	size_t sentBytes = 0;
	while (sentBytes < count)
	{
		size_t chunkSize = count - sentBytes;
		if (chunkSize > 1024)
		{
			chunkSize = 1024;
		}
		chunkSize = file.read(buffer, chunkSize);
		webServer->sendContent_P((char *)buffer, chunkSize);
		sentBytes += chunkSize;
	}

	file.close();
}

/**
 * @brief Clear the log file of the controller.
 */
void TesLight::LogEndpoint::clearLog()
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to clear the log file."));
	TesLight::Logger::clearLog();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	webServer->send(200, F("text/plain"), F("Log cleared."));
}