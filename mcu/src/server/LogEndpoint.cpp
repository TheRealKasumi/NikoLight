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
 * @brief Add all request handler for this {@link TesLight::RestEndpoint} to the {@link TesLight::WebServer}.
 */
void TesLight::LogEndpoint::begin(FS *_fileSystem)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize Log Endpoint."));
	fileSystem = _fileSystem;

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Register Log Endpoints."));
	getWebServer()->addRequestHandler((getBaseUri() + F("log/size")).c_str(), http_method::HTTP_GET, TesLight::LogEndpoint::getLogSize);
	getWebServer()->addRequestHandler((getBaseUri() + F("log")).c_str(), http_method::HTTP_GET, TesLight::LogEndpoint::getLog);
	// TODO: Why delete method isn't working? Bug in library or expected? Until resolved patch is used for deleting... :(
	getWebServer()->addRequestHandler((getBaseUri() + F("log")).c_str(), http_method::HTTP_PATCH, TesLight::LogEndpoint::clearLog);
}

/**
 * @brief Return the size of the log file in bytes.
 * @param request instance of {@link AsyncWebServerRequest}
 */
void TesLight::LogEndpoint::getLogSize(AsyncWebServerRequest *request)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get the log size."));
	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Open log file."));
	File file = fileSystem->open(LOG_FILE_NAME, FILE_READ);
	if (!file)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open log file."));
		request->send(500, F("text/plain"), F("Failed to open log file."));
		return;
	}
	else if (file.isDirectory())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open log file because it is a directory."));
		file.close();
		request->send(500, F("text/plain"), F("Failed to open log file because it is a directory."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Reading log size in bytes and closing file."));
	const size_t logSize = file.size();
	file.close();

	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	request->send(200, F("text/plain"), String(logSize));
}

/**
 * @brief Get a section of the log file, determinded by the paremters start and count in bytes.
 * @param request instance of {@link AsyncWebServerRequest}
 */
void TesLight::LogEndpoint::getLog(AsyncWebServerRequest *request)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to get a section of the log file."));
	if (request->arg(F("start")).length() == 0 || request->arg(F("count")).length() == 0)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The parameters \"start\" and \"count\" must be provided."));
		request->send(400, F("text/plain"), F("The parameters \"start\" and \"count\" must be provided."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Reading start and count parameters."));
	const size_t start = request->arg(F("start")).toInt();
	const size_t count = request->arg(F("count")).toInt();

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Open log file."));
	File file = fileSystem->open(LOG_FILE_NAME, FILE_READ);
	if (!file)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open log file."));
		request->send(500, F("text/plain"), F("Failed to open log file."));
		return;
	}
	else if (file.isDirectory())
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to open log file because it is a directory."));
		file.close();
		request->send(500, F("text/plain"), F("Failed to open log file because it is a directory."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Reading log size in bytes and closing file."));
	const size_t logSize = file.size();
	file.close();

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Checking start and count parameters."));
	if (start >= logSize || start + count >= logSize)
	{
		TesLight::Logger::log(TesLight::Logger::LogLevel::WARN, SOURCE_LOCATION, F("The start or count parameters are invalid."));
		request->send(400, F("text/plain"), F("The start or count parameters are invalid."));
		return;
	}

	TesLight::Logger::log(TesLight::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Sending chunked response."));
	AsyncWebServerResponse *response = request->beginChunkedResponse("text/plain", [start, count](uint8_t *buffer, size_t maxLen, size_t index) -> size_t
																	 {
		esp_task_wdt_reset();
		size_t remaining = count - index;
		if(remaining == 0)
		{
			return 0;
		} 
		else if(remaining > maxLen) 
		{
			remaining  = maxLen;
		} 

		File file = fileSystem->open(LOG_FILE_NAME, FILE_READ);
		if (!file)
		{
			return 0;
		}

		file.seek(start + index);
		size_t readBytes = file.read(buffer, remaining);
		file.close();

		return readBytes; });
	request->send(response);
}

/**
 * @brief Clear the log file of the controller.
 * @param request instance of {@link AsyncWebServerRequest}
 */
void TesLight::LogEndpoint::clearLog(AsyncWebServerRequest *request)
{
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Received request to clear the log file."));
	TesLight::Logger::clearLog();
	TesLight::Logger::log(TesLight::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Sending the response."));
	request->send(200, F("text/plain"), F("Log cleared."));
}