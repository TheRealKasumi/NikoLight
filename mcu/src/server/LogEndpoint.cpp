/**
 * @file LogEndpoint.cpp
 * @author TheRealKasumi
 * @brief Implementation of a REST endpoint to manage the log of the NikoLight controller.
 *
 * @copyright Copyright (c) 2023 TheRealKasumi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "server/LogEndpoint.h"

// Initialize
FS *NL::LogEndpoint::fileSystem = nullptr;

/**
 * @brief Add all request handler for this {@link NL::RestEndpoint} to the {@link NL::WebServerManager}.
 */
void NL::LogEndpoint::begin(FS *_fileSystem)
{
	NL::LogEndpoint::fileSystem = _fileSystem;
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("log/size")).c_str(), http_method::HTTP_GET, NL::LogEndpoint::getLogSize);
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("log")).c_str(), http_method::HTTP_GET, NL::LogEndpoint::getLog);
	NL::WebServerManager::addRequestHandler((getBaseUri() + F("log")).c_str(), http_method::HTTP_DELETE, NL::LogEndpoint::clearLog);
}

/**
 * @brief Return the size of the log file in bytes.
 */
void NL::LogEndpoint::getLogSize()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the log size."));
	File file = NL::LogEndpoint::fileSystem->open(LOG_FILE_NAME, FILE_READ);
	if (!file)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open log file."));
		NL::LogEndpoint::sendSimpleResponse(500, F("Failed to open log file."));
		return;
	}
	else if (file.isDirectory())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open log file because it is a directory."));
		file.close();
		NL::LogEndpoint::sendSimpleResponse(500, F("Failed to open log file because it is a directory."));
		return;
	}

	const size_t logSize = file.size();
	file.close();

	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	DynamicJsonDocument jsonDoc(1024);
	const JsonObject log = jsonDoc.createNestedObject(F("log"));
	log[F("size")] = logSize;
	NL::LogEndpoint::sendJsonDocument(200, F("This is my current log size."), jsonDoc);
}

/**
 * @brief Get a section of the log file, determinded by the paremters start and count in bytes.
 */
void NL::LogEndpoint::getLog()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get a section of the log file."));
	if (!NL::LogEndpoint::webServer->hasArg(F("start")) || NL::LogEndpoint::webServer->arg(F("start")).length() == 0 || !NL::LogEndpoint::webServer->hasArg(F("count")) || NL::LogEndpoint::webServer->arg(F("count")).length() == 0)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The parameters \"start\" and \"count\" must be provided."));
		NL::LogEndpoint::sendSimpleResponse(400, F("The url parameters \"start\" and \"count\" must be provided."));
		return;
	}

	File file = NL::LogEndpoint::fileSystem->open(LOG_FILE_NAME, FILE_READ);
	if (!file)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open log file."));
		NL::LogEndpoint::sendSimpleResponse(500, F("Failed to open log file."));
		return;
	}
	else if (file.isDirectory())
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open log file because it is a directory."));
		file.close();
		NL::LogEndpoint::sendSimpleResponse(500, F("Failed to open log file because it is a directory."));
		return;
	}

	const size_t start = NL::LogEndpoint::webServer->arg(F("start")).toInt();
	const size_t count = NL::LogEndpoint::webServer->arg(F("count")).toInt();
	const size_t logSize = file.size();
	if (start > logSize || start + count > logSize)
	{
		NL::Logger::log(NL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The start or count parameters are invalid."));
		file.close();
		NL::LogEndpoint::sendSimpleResponse(400, F("The start or count parameters are invalid."));
		return;
	}

	NL::LogEndpoint::webServer->setContentLength(count);
	NL::LogEndpoint::webServer->send(200, F("text/plain"), String());

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
		NL::LogEndpoint::webServer->sendContent_P((char *)buffer, chunkSize);
		sentBytes += chunkSize;
	}

	file.close();
}

/**
 * @brief Clear the log file of the controller.
 */
void NL::LogEndpoint::clearLog()
{
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to clear the log file."));
	NL::Logger::clearLog();
	NL::Logger::log(NL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	NL::LogEndpoint::sendSimpleResponse(200, F("All clean. Can I now have a cookie please?"));
}